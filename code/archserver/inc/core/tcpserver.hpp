#pragma once
#include "pre-req.hpp"
#include "uv.h"
#include "connmap.hpp"
#include "buffer.hpp"
#include "protocol.hpp"
#include "protocol-queue.hpp"
#include "pipeclient.hpp"

namespace core
{

	class TCPServer
	{
		UNCOPYABLE(TCPServer);
		UNMOVABLE(TCPServer);

	public:
		typedef struct _tcp_t : public uv_tcp_t
		{
			_tcp_t(
				TCPServer& server,
				conn_id_t connection_id,
				Connection<_tcp_t>* connection)
				: uv_tcp_t({})
				, svr(server)
				, conn_id(connection_id)
				, conn(connection)
			{}

			// extended data:
			TCPServer&			svr;
			conn_id_t			conn_id;
			Connection<_tcp_t>*	conn;
		} tcp_t;

		typedef struct _uvtimer_t : public uv_timer_t
		{
			_uvtimer_t(TCPServer& server)
				: uv_timer_t({ 0 })
				, svr(server)
			{}

			TCPServer& svr;
		} uvtimer_t;

		typedef struct _buf_t : public uv_buf_t
		{
			UNCOPYABLE(_buf_t);
			UNMOVABLE(_buf_t);

			_buf_t()
				: uv_buf_t({})
			{}

			void resize(size_t size)
			{
				buffer.resize(size);
				base = (char*)buffer.buffer();
				len = (ULONG)buffer.length();
			}

			Buffer	buffer;
		} _buf_t;

		typedef struct _write_req_t : public uv_write_t
		{
			_write_req_t(
				std::vector<uint8_t>&& stream_data,
				uint16_t ccflags)
				: data(std::move(stream_data))
				, ccf(ccflags)
			{
				buf.base = (char*)data.data();
				buf.len = (ULONG)data.size();
			}

			std::vector<uint8_t>	data;
			uv_buf_t				buf;
			uint16_t				ccf;
		} write_req_t;

	public:
		TCPServer(
			const std::string& ipaddr,
			uint16_t port,
			int backlog);
		~TCPServer();

	public:
		void wait();

	private:
		void _work_thread();
		void _ensure_protocol_data(Connection<tcp_t>& conn);
		archproto::IProtocolHandler* _get_protocol_handler(archproto::ProtocolType ptype);
		void _switch_protocol(Connection<tcp_t>& conn);

	private:
		std::string		_ipaddr;
		uint16_t		_port;
		int				_backlog;

		uv_loop_t		_uvloop;
		tcp_t			_tcp_handle;
		uvtimer_t		_tm_write;
		ConnMap<_tcp_t>	_connections;
		std::array<std::unique_ptr<archproto::IProtocolHandler>, archproto::PT_ProtoTypesNum> _proto_handlers;
		std::vector<core::ProtocolQueue::node_t>	_tmp_outnodes;

	private:
		std::thread		_thread;
		bool			_quit;

	private:
		static void _on_connect(uv_stream_t* stream, int status);
		static void _on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
		static void _on_alloc_read_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
		static void _on_closed(uv_handle_t* handle);
		static void _on_write_timer(uv_timer_t* handle);
		static void	_on_written(uv_write_t* req, int status);
		static void _close_connection(tcp_t* handle);
	};

}







