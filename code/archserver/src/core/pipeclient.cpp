#include "pipeclient.hpp"
#include "protocol-arch.hpp"

using namespace core;
using namespace archproto;

core::PipeClient::PipeClient(
		const std::string& name,
		const std::shared_ptr<ProtocolQueue>& inque,
		const std::shared_ptr<ProtocolQueue>& outque,
		const outque_guard_t& guard)
	: _thread(std::bind(&PipeClient::_workthread, this))
	, _inque(inque)
	, _outque(outque)
	, _guard(guard)
	, _pipe_handle(*this)
	, _uvloop({})
	, _async_write(*this)
	, _uvtm_reconn(*this)
	, _proto_handlers{
		/*PT_Http*/ nullptr,
		/*PT_WebSocket */ nullptr,
		/*PT_Arch*/ std::make_unique<ArchProtocol>()}
	, _tempinfo_ready(false)
{
#if defined(WIN32) || defined(_WIN32)
	_name = "\\\\?\\pipe\\" + name + ".sock";
#else
	_name = "/tmp/" + name + ".sock";
#endif
}

core::PipeClient::~PipeClient()
{}

void core::PipeClient::_workthread()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	uv_loop_init(&_uvloop);
	uv_pipe_init(&_uvloop, &_pipe_handle, 0);
	uv_timer_init(&_uvloop, &_uvtm_reconn);

	uv_async_init(&_uvloop, &_async_write, _on_write);

	conn_t* conn = new conn_t(*this);
	uv_pipe_connect(conn, &_pipe_handle, _name.c_str(), _on_connect);
	uv_run(&_uvloop, UV_RUN_DEFAULT);
}


void core::PipeClient::send(core::ProtocolQueue::node_t&& node)
{
	_outque->push(std::move(node));
	uv_async_send(&_async_write);
}


void core::PipeClient::_on_connect(uv_connect_t* conn, int status)
{
	conn_t* connection = (conn_t*)conn;
	if(status >= 0)
	{
		// reset connection environments
		connection->pipcli._tempinfo.reset();
		connection->pipcli._tempinfo_ready = false;

		// start to read data
		connection->uvstream = conn->handle;
		uv_read_start(
			conn->handle,
			_on_alloc,
			_on_read);
	}
	else
	{
		uv_timer_start(
			&connection->pipcli._uvtm_reconn,
			_on_tm_reconn,
			5000,
			0);
		delete connection;
	}
}

void core::PipeClient::_on_tm_reconn(uv_timer_t* handle)
{
	tm_reconn_t& reconn_handle = (tm_reconn_t&)* handle;

	conn_t* conn = new conn_t(reconn_handle.pipcli);
	uv_pipe_connect(
		conn,
		&reconn_handle.pipcli._pipe_handle,
		reconn_handle.pipcli._name.c_str(),
		_on_connect);
}

void core::PipeClient::_on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{ // read responses from service instance
	pipe_t& pipe_handle = (pipe_t&)*client;

	if (nread > 0)
	{
		size_t offset = 0;
		bool goon = true;
		while (goon && ((ssize_t)offset< nread))
		{
			size_t procbytes = 0;
			goon = pipe_handle.pipcli._read(
				(uint8_t*)buf->base + offset,
				nread - offset,
				procbytes);
			offset += procbytes;
		}

		if (goon)
		{
			uv_read_start(
				(uv_stream_t*)& pipe_handle,
				_on_alloc,
				_on_read);
		}
		else
		{
			uv_close((uv_handle_t*)client, _on_closed);
		}
	}
	else if (nread < 0)
	{
		uv_close((uv_handle_t*)client, _on_closed);
	}

	delete buf->base;
}

bool core::PipeClient::_read(
	const uint8_t* buf,
	size_t toreadlen,
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
			toreadlen,
			procbytes
		);

		switch (proc_ret)
		{
		case IProtocolHandler::PPR_AGAIN:
			break;

		case IProtocolHandler::PPR_PULSE:
		{
			// prepare a queue node
			core::ProtocolQueue::node_t node(
				std::move(_tempinfo.protocol_data),
				(uint16_t)_tempinfo.conn_id.value(),
				(uint16_t)_tempinfo.ccf.value(),
				(ProtocolType)_tempinfo.protocol_type.value());

			// make the node have guarded values
			_guard(node);

			// push the node into a queue
			_inque->push(std::move(node));

			// expire the temp info
			_tempinfo.reset();
			_tempinfo_ready = false;
		}
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
			toreadlen,
			_tempinfo_ready,
			procbytes);
	}

	return goon;
}

bool core::PipeClient::_conn_tempinfo_s::parse(
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

void core::PipeClient::_conn_tempinfo_s::ensure_data()
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

void core::PipeClient::_conn_tempinfo_s::reset()
{
	pp = PP_Idle;
	conn_id.value(0);
	ccf.value(0);
	protocol_type.value(PT_Unknown);
	protocol_data = nullptr;
}


void core::PipeClient::_on_write(uv_async_t* async)
{
	async_t& async_handle = *(async_t*)async;
	
	std::vector<ProtocolQueue::node_t> nodes;
	if (async_handle.pipcli._outque->pop(nodes))
	{
		for (auto& node : nodes)
		{
			std::vector<uint8_t> stream;

			// serialize node info
			_ser_queue_node(stream, node);

			// serialize protocol data
			IProtocolHandler* phdl = async_handle.pipcli._get_protocol_handler(node.proto);
			if (phdl->ser_service_stream(stream, *node.data))
			{
				write_req_t* req = new write_req_t(std::move(stream));
				uv_write(
					req,
					(uv_stream_t*)& async_handle.pipcli._pipe_handle,
					&req->uvbuf,
					1,
					_on_written);
			}
		}
	}
}

void core::PipeClient::_ser_queue_node(
			std::vector<uint8_t>& dest,
			const ProtocolQueue::node_t& node)
{
	dest << VUInt(node.conn_id);
	dest << VUInt(node.ccf);
	dest << VUInt(node.proto);
}

void core::PipeClient::_on_written(uv_write_t* req, int status)
{
	delete (write_req_t*)req;
}

void core::PipeClient::_on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	buf->base = new char[suggested_size];
	buf->len = (ULONG)suggested_size;
}

void core::PipeClient::_on_closed(uv_handle_t* handle)
{}

//void core::PipeClient::_ensure_tempdata(ProtocolType ptype)
//{
//	if (_tempdata == nullptr)
//	{
//		switch (ptype)
//		{
//		case PT_Arch:
//			_tempdata = std::make_unique<ipro::protocol_arch::ArchProtocolData>();
//			break;
//
//		default:
//			assert(false);
//		}
//	}
//}

IProtocolHandler* core::PipeClient::_get_protocol_handler(ProtocolType ptype) const noexcept
{
	return _proto_handlers[ptype].get();
}



