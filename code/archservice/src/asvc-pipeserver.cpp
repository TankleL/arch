#include <cassert>
#include "asvc-pipeserver.hpp"
#include "protocol-arch.hpp"

using namespace archproto;
using namespace archsvc;

archsvc::PipeServer::PipeServer(
	const std::string& pipename,
	const receiver_t& receiver) noexcept
	: _pipe_server(*this)
	, _pipe_client(*this)
	, _uvloop({})
	, _receiver(receiver)
	, _accepted(false)
	, _proto_handlers{
		/*PT_Http*/ nullptr,
		/*PT_WebSocket */ nullptr,
		/*PT_Arch*/ std::make_unique<ArchProtocol>()}
	, _thread(std::thread(std::bind(&PipeServer::_workthread, this)))
	, _tempinfo_ready(false)
{
#if defined (WIN32) || defined(_WIN32)
	_pipename = "\\\\?\\pipe\\" + pipename + ".sock";
#else
	_pipename = "/tmp/" + pipename + ".sock";
#endif
}

archsvc::PipeServer::~PipeServer()
{}

void archsvc::PipeServer::write(
	uint16_t conn_id,
	uint16_t ccf,
	ProtocolType proto,
	std::unique_ptr<archproto::IProtocolData>&& data)
{
	if (_accepted)
	{
		std::vector<uint8_t> stream;
		stream << VUInt(conn_id);
		stream << VUInt(ccf);
		stream << VUInt(proto);

		IProtocolHandler* phdl = _get_protocol_handler(proto);
		phdl->ser_service_stream(stream, *data);

		write_req_t* req = new write_req_t(std::move(stream));
		uv_write(req, (uv_stream_t*)& _pipe_client, &req->buf, 1, _on_written);
	}
}

void archsvc::PipeServer::wait()
{
	_thread.join();
}

void archsvc::PipeServer::_workthread()
{
	uv_loop_init(&_uvloop);
	uv_pipe_init(&_uvloop, &_pipe_server, 0);

	uv_pipe_bind(&_pipe_server, _pipename.c_str());
	uv_listen((uv_stream_t*)&_pipe_server, 100, _on_connect);

	uv_run(&_uvloop, UV_RUN_DEFAULT);
}

void archsvc::PipeServer::_on_connect(uv_stream_t* stream, int status)
{
	pipe_t& pipe_handle = (pipe_t&)(*stream);
	if (status < 0 ||
		pipe_handle.pipesvr._accepted)
	{
		return;
	}
	
	uv_pipe_init(
		&pipe_handle.pipesvr._uvloop,
		&pipe_handle.pipesvr._pipe_client,
		0);
	if (!uv_accept(stream, (uv_stream_t*)&pipe_handle.pipesvr._pipe_client))
	{
		pipe_handle.pipesvr._accepted = true;
		pipe_handle.pipesvr._tempinfo.reset();
		pipe_handle.pipesvr._tempinfo_ready = false;
		uv_read_start((uv_stream_t*)&pipe_handle.pipesvr._pipe_client, _on_alloc, _on_read);
	}
	else
	{
		uv_close((uv_handle_t*)&pipe_handle.pipesvr._pipe_client, _on_closed);
	}
}

void archsvc::PipeServer::_on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
	if (nread > 0)
	{
		pipe_t& pipe_handler = (pipe_t&)*client;

		size_t offset = 0;
		bool goon = true;
		while (goon && ((ssize_t)offset < nread))
		{
			size_t procbytes = 0;

			goon = pipe_handler.pipesvr._read(
				(uint8_t*)buf->base + offset,
				nread - offset,
				procbytes);
			offset += procbytes;
		}
	}
	else if (nread < 0)
	{
		uv_close((uv_handle_t*)client, _on_closed);
	}
	delete buf->base;
}

bool archsvc::PipeServer::_read(
	const uint8_t* buf,
	size_t nread, 
	size_t& procbytes)
{
	bool goon = true;
	
	if (_tempinfo_ready)
	{
		IProtocolHandler* phdl = _get_protocol_handler(
			(ProtocolType)_tempinfo.protocol_type.value());
		IProtocolHandler::ProtoProcRet proc_ret = phdl->des_service_stream(
			*_tempinfo.protocol_data,
			buf,
			nread,
			procbytes);

		switch (proc_ret)
		{
		case IProtocolHandler::PPR_AGAIN:
			break;

		case IProtocolHandler::PPR_PULSE:

			if (!_receiver(
				(uint16_t)_tempinfo.conn_id.value(),
				(uint16_t)_tempinfo.ccf.value(),
				(ProtocolType)_tempinfo.protocol_type.value(),
				std::move(_tempinfo.protocol_data),
				*this))
			{
				goon = false;
			}

			// expire the temp info
			_tempinfo.reset();
			_tempinfo_ready = false;
			break;

		case IProtocolHandler::PPR_ERROR:
		case IProtocolHandler::PPR_CLOSE:
		default:
			goon = false;
		}
	}
	else
	{
		goon = _tempinfo.parse(
			buf,
			nread,
			_tempinfo_ready,
			procbytes);
	}

	return goon;
}

bool archsvc::PipeServer::_conn_tempinfo_s::parse(
	const uint8_t* buf,
	size_t toreadlen,
	bool& ready,
	size_t& procbytes)
{
	ready = false;
	VUInt::DigestStatus digst;
	while (procbytes < toreadlen)
	{
		switch (pp)
		{
		case PP_Idle: // Connection ID
			digst = conn_id.digest(*(buf + procbytes++));
			if (VUInt::DS_Idle == digst)
			{
				pp = PP_CCF;
			}
			else if (VUInt::DS_Bad == digst)
			{
				return false;
			}
			break;

		case PP_CCF:
			digst = ccf.digest(*(buf + procbytes++));
			if (VUInt::DS_Idle == digst)
			{
				pp = PP_Proto;
			}
			else if (VUInt::DS_Bad == digst)
			{
				return false;
			}
			break;

		case PP_Proto:
			digst = protocol_type.digest(*(buf + procbytes++));
			if (VUInt::DS_Idle == digst)
			{
				ready = true;
				pp = PP_Idle;
				ensure_data();
				return true; // Return Here.
			}
			else if (VUInt::DS_Bad == digst)
			{
				return false;
			}
			break;

		default:
			return false;
		}
	}
	return true;
}

void archsvc::PipeServer::_conn_tempinfo_s::ensure_data()
{
	assert(protocol_type.value() < PT_ProtoTypesNum);

	switch (protocol_type.value())
	{
	case PT_Http:
		assert(false); // not being implemented
		break;

	case PT_WebSocket:
		assert(false); // not being implemented
		
	case PT_Arch:
		protocol_data = std::make_unique<ArchProtocolData>();
		break;
	}
}

void archsvc::PipeServer::_conn_tempinfo_s::reset()
{
	pp = PP_Idle;
	conn_id.value(0);
	ccf.value(0);
	protocol_type.value(PT_Unknown);
	protocol_data = nullptr;
}

void archsvc::PipeServer::_on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	buf->base = new char[suggested_size];
	buf->len = (ULONG)suggested_size;
}

void archsvc::PipeServer::_on_written(uv_write_t* req, int status)
{
	delete (write_req_t*)req;
}

void archsvc::PipeServer::_on_closed(uv_handle_t* handle)
{
	pipe_t& pipe_handler = (pipe_t&)*handle;
	pipe_handler.pipesvr._accepted = false;
}

IProtocolHandler* archsvc::PipeServer::_get_protocol_handler(ProtocolType proto) const noexcept
{
	assert(proto < PT_ProtoTypesNum);
	return _proto_handlers[proto].get();
}



