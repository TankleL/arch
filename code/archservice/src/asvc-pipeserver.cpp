#include "asvc-pipeserver.hpp"

using namespace archsvc;

archsvc::PipeServer::PipeServer(
	const std::string& pipename) noexcept
	: _pipe_handle(*this)
	, _uvloop({})
	, _thread(std::thread(std::bind(&PipeServer::_workthread, this)))
{
#if defined (WIN32) || defined(_WIN32)
	_pipename = "\\\\?\\pipe\\" + pipename + ".sock";
#else
	_pipename = "/tmp/" + pipename + ".sock";
#endif
}

archsvc::PipeServer::~PipeServer()
{}

void archsvc::PipeServer::wait()
{
	_thread.join();
}

void archsvc::PipeServer::_workthread()
{
	uv_loop_init(&_uvloop);
	uv_pipe_init(&_uvloop, &_pipe_handle, 0);

	uv_pipe_bind(&_pipe_handle, _pipename.c_str());
	uv_listen((uv_stream_t*)& _pipe_handle, 100, _on_connect);

	uv_run(&_uvloop, UV_RUN_DEFAULT);
}

void archsvc::PipeServer::_on_connect(uv_stream_t* stream, int status)
{
	if (status < 0)
	{
		return;
	}

	pipe_t& pipe_handle = (pipe_t&)(*stream);
	pipe_t* client = new pipe_t(pipe_handle.pipcli);
	uv_pipe_init(&pipe_handle.pipcli._uvloop, client, 0);
	if (!uv_accept(stream, (uv_stream_t*)client))
	{
		uv_read_start((uv_stream_t*)client, _on_alloc, _on_read);
	}
	else
	{
		uv_close((uv_handle_t*)client, _on_closed);
	}
}

void archsvc::PipeServer::_on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
	if (nread > 0)
	{
		pipe_t& pipe_handler = (pipe_t&)client;
		pipe_handler.pipcli._dataproc.deserialize(
			(const uint8_t*)buf->base,
			(size_t)nread);
	}
	else if (nread < 0)
	{
		uv_close((uv_handle_t*)client, _on_closed);
	}
	delete buf->base;
}

void archsvc::PipeServer::_on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	buf->base = new char[suggested_size];
	buf->len = (ULONG)suggested_size;
}

void archsvc::PipeServer::_on_closed(uv_handle_t* handle)
{
	delete handle;
}




