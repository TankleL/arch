#pragma once

#include "pre-req.hpp"
#include "protocol-queue.hpp"
#include "uv.h"
#include "rawsvcdata-handler.hpp"

namespace core
{

	class PipeClient
	{
		UNCOPYABLE(PipeClient);
		UNMOVABLE(PipeClient);
	public:
		typedef struct _pipe_t : public uv_pipe_t
		{
			_pipe_t(PipeClient& pipeclient)
				: pipcli(pipeclient)
			{}

			PipeClient&		pipcli;
		} pipe_t;

		typedef struct _conn_t : public uv_connect_t
		{
			_conn_t(PipeClient& pipeclient)
				: pipcli(pipeclient)
				, uvstream(nullptr)
			{}

			PipeClient& pipcli;
			uv_stream_t*	uvstream;
		} conn_t;

		typedef struct _async_t : public uv_async_t
		{
			_async_t(PipeClient& pipeclient)
				: pipcli(pipeclient)
			{}

			PipeClient& pipcli;
		} async_t;

		typedef struct _write_req_t : public uv_write_t
		{
			_write_req_t(std::vector<std::uint8_t>&& stream_data)
				: uv_write_t({0})
				, data(std::move(stream_data))
			{
				uvbuf.base = (char*)data.data();
				uvbuf.len = (ULONG)data.size();
			}

			std::vector<std::uint8_t>	data;
			uv_buf_t					uvbuf;
		} write_req_t;

	public:
		PipeClient(
			const std::string& name,
			const std::shared_ptr<ProtocolQueue>& inque,
			const std::shared_ptr<ProtocolQueue>& outque);
		~PipeClient();

		void send(core::ProtocolQueue::node_t&& node);

	private:
		void _workthread();

	private:
		std::thread		_thread;
		std::string		_name;
		std::shared_ptr<ProtocolQueue>	_inque;
		std::shared_ptr<ProtocolQueue>	_outque;

		pipe_t			_pipe_handle;
		uv_loop_t		_uvloop;
		async_t			_async_write;
		svc::RawSvcDataHandler	_dataproc;

	private:
		static void _on_connect(uv_connect_t* conn, int status);
		static void _on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
		static void _on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
		static void _on_write(uv_async_t* handle);
		static void	_on_written(uv_write_t* req, int status);
		static void _on_closed(uv_handle_t* handle);
	};


}






