#include "pipeclient.hpp"

using namespace core;

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
{
	pipe_t& pipe_handle = (pipe_t&)*client;

	if (nread > 0)
	{
		size_t offset = 0;
		bool goon = true;
		while (goon && ((ssize_t)offset< nread))
		{
			size_t procbytes = 0;
			svc::RawSvcDataHandler::ParsingPhase pp =
				pipe_handle.pipcli._dataproc.deserialize(
					(uint8_t*)buf->base + offset,
					nread - offset,
					procbytes);
			offset = procbytes;

			if (svc::RawSvcDataHandler::PP_Idle == pp)
			{
				core::ProtocolQueue::node_t node;
				pipe_handle.pipcli._dataproc.get_deserialized(node);
				pipe_handle.pipcli._guard(node);
				pipe_handle.pipcli._inque->push(std::move(node));

				uv_read_start(
					(uv_stream_t*)& pipe_handle,
					_on_alloc,
					_on_read);
			}
			else if(svc::RawSvcDataHandler::PP_Bad == pp)
			{
				goon = false;
				uv_close((uv_handle_t*)client, _on_closed);
			}
		}
	}
	else if (nread < 0)
	{
		uv_close((uv_handle_t*)client, _on_closed);
	}

	delete buf->base;
}

void core::PipeClient::_on_write(uv_async_t* async)
{
	async_t& async_handle = *(async_t*)async;
	
	ProtocolQueue::node_t node;
	if (async_handle.pipcli._outque->pop(node))
	{
		std::vector<uint8_t>	stream_data;
		if (async_handle.pipcli._dataproc.serialize(
				stream_data,
				node))
		{
			write_req_t* req = new write_req_t(std::move(stream_data));
			uv_write(
				req,
				(uv_stream_t*)& async_handle.pipcli._pipe_handle,
				&req->uvbuf,
				1,
				_on_written);
		}
	}
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




