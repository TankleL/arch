#include "pipeclient.hpp"

using namespace core;

core::PipeClient::PipeClient(
		const std::string& name,
		const std::shared_ptr<ProtocolQueue>& inque,
		const std::shared_ptr<ProtocolQueue>& outque)
	: _thread(std::bind(&PipeClient::_workthread, this))
	, _inque(inque)
	, _outque(outque)
	, _pipe_handle(*this)
	, _uvloop({})
	, _async_write(*this)
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
	uv_loop_init(&_uvloop);
	uv_pipe_init(&_uvloop, (uv_pipe_t*)& _pipe_handle, 1);

	uv_async_init(&_uvloop, &_async_write, _on_write);

	conn_t conn(*this);
	uv_pipe_connect(&conn, &_pipe_handle, _name.c_str(), _on_connect);
	uv_run(&_uvloop, UV_RUN_DEFAULT);
}


void core::PipeClient::send(core::ProtocolQueue::node_t&& node)
{
	_outque->push(std::move(node));
	uv_async_send(&_async_write);
}


void core::PipeClient::_on_connect(uv_connect_t* conn, int status)
{
	if(status >= 0)
	{
		conn_t* connection = (conn_t*)conn;
		connection->uvstream = conn->handle;
		uv_read_start(
			conn->handle,
			_on_alloc,
			_on_read);
	}
	else
	{
		// TODO: handle connection failures.
	}
}

void core::PipeClient::_on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
	if (nread > 0)
	{
		pipe_t& pipe_handle = (pipe_t&)client;

		if (pipe_handle.pipcli._dataproc.deserialize(
				(uint8_t*)buf->base,
				nread))
		{
			core::ProtocolQueue::node_t node;
			pipe_handle.pipcli._dataproc.get_deserialized(node);

			pipe_handle.pipcli._inque->push(std::move(node));

			uv_read_start(
				(uv_stream_t*)& pipe_handle,
				_on_alloc,
				_on_read);
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
		if (async_handle.pipcli._dataproc.serialize(stream_data, node))
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
{
	delete handle;
}




