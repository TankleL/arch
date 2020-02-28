#include "asvc-pipeserver.hpp"
#include <iostream>

using namespace archsvc;

archsvc::PipeServer::PipeServer(
	const std::string& pipename,
	const receiver_t& receiver) noexcept
	: _pipe_server(*this)
	, _pipe_client(*this)
	, _uvloop({})
	, _receiver(receiver)
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

void archsvc::PipeServer::write(std::vector<uint8_t>&& data)
{
	write_req_t* req = new write_req_t(std::move(data));
	uv_write(req, (uv_stream_t*)&_pipe_client, &req->buf, 1, _on_written);
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
	if (status < 0)
	{
		return;
	}

	pipe_t& pipe_handle = (pipe_t&)(*stream);
	pipe_t* client = new pipe_t(pipe_handle.pipesvr);
	uv_pipe_init(&pipe_handle.pipesvr._uvloop, client, 0);
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
		pipe_t& pipe_handler = (pipe_t&)*client;

		size_t offset = 0;
		bool goon = true;
		while (goon && ((ssize_t)offset < nread))
		{
			size_t procbytes = 0;
			RawSvcDataHandler::ParsingPhase pp =
				pipe_handler.pipesvr._dataproc.deserialize(
					(uint8_t*)buf->base + offset,
					nread - offset,
					procbytes);
			offset += procbytes;

			if (RawSvcDataHandler::PP_Idle == pp)
			{
				std::vector<uint8_t> data;
				pipe_handler.pipesvr._dataproc.get_deserialized(data);
				
				if (!pipe_handler.pipesvr._receiver(
					std::move(data),
					pipe_handler.pipesvr))
				{
					goon = false;
					uv_close((uv_handle_t*)client, _on_closed);
				}
			}
			else if (RawSvcDataHandler::PP_Bad == pp)
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
	delete handle;
}




