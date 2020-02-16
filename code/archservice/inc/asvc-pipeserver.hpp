#pragma once
#include "uv.h"
#include <thread>
#include <functional>
#include <cstdint>

#include "asvc-rawsvcdata-handler.hpp"

namespace archsvc
{

	class PipeServer
	{
	public:
		typedef struct _pipe_t : public uv_pipe_t
		{
			_pipe_t(PipeServer& pipeclient)
				: pipcli(pipeclient)
			{}

			PipeServer& pipcli;
		} pipe_t;

	public:
		PipeServer(
			const std::string& pipename) noexcept;
		~PipeServer();

		void wait();

	private:
		void _workthread();

	private:
		pipe_t		_pipe_handle;
		uv_loop_t	_uvloop;
		std::string _pipename;
		RawSvcDataHandler	_dataproc;

	private:
		std::thread	_thread;

	private:
		static void _on_connect(uv_stream_t* stream, int status);
		static void _on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
		static void _on_write(uv_write_t* req, int status);
		static void _on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
		static void _on_closed(uv_handle_t* handle);
		static void _free(uv_write_t* req);
	};

}




