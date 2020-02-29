#pragma once
#include "uv.h"
#include <thread>
#include <functional>
#include <cstdint>
#include <vector>

#include "asvc-rawsvcdata-handler.hpp"

namespace archsvc
{

	class PipeServer
	{
	public:
		typedef struct _pipe_t : public uv_pipe_t
		{
			_pipe_t(PipeServer& pipesvr)
				: pipesvr(pipesvr)
			{}

			PipeServer&	pipesvr;
		} pipe_t;

		typedef std::function<
			bool(
				std::vector<uint8_t>&& data,
				uint16_t conn_id,
				uint16_t ccf,
				PipeServer& pipe)> receiver_t;

		typedef struct _write_req_t : public uv_write_t
		{
			_write_req_t(std::vector<uint8_t>&& senddata)
				: uv_write_t({ 0 })
				, data(std::move(senddata))
			{
				buf.base = (char*)data.data();
				buf.len = (ULONG)data.size();
			}

			std::vector<uint8_t>	data;
			uv_buf_t				buf;
		} write_req_t;

	public:
		PipeServer(
			const std::string& pipename,
			const receiver_t& receiver) noexcept;
		~PipeServer();

		void write(
			std::vector<uint8_t>&& data,
			uint16_t conn_id,
			uint16_t ccf);
		void wait();

	private:
		void _workthread();

	private:
		pipe_t				_pipe_server;
		pipe_t				_pipe_client;
		uv_loop_t			_uvloop;
		std::string			_pipename;
		RawSvcDataHandler	_dataproc;

		receiver_t			_receiver;
		bool				_accepted;

	private:
		std::thread	_thread;

	private:
		static void _on_connect(uv_stream_t* stream, int status);
		static void _on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
		static void _on_written(uv_write_t* req, int status);
		static void _on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
		static void _on_closed(uv_handle_t* handle);
		static void _free(uv_write_t* req);
	};

}




