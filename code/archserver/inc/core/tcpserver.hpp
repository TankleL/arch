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

		typedef struct _buf_t : public uv_buf_t
		{
			UNCOPYABLE(_buf_t);

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

	public:
		TCPServer(
			int svc_inst_count,
			const std::string& ipaddr,
			uint16_t port,
			int backlog);
		~TCPServer();

	public:
		void wait();

	private:
		void _work_thread();
		void _close_connection(const Connection<tcp_t>& conn);
		bool _ensure_protocol_data(Connection<tcp_t>& conn);
		IProtocolHandler* _get_protocol_handler(ProtocolType ptype);
		void _switch_protocol(Connection<tcp_t>& conn);

	private:
		std::string		_ipaddr;
		uint16_t		_port;
		int				_backlog;
		int				_svc_inst_count;

		uv_loop_t		_uvloop;
		tcp_t			_tcp_handle;
		ConnMap<_tcp_t>	_connections;
		std::array<std::unique_ptr<IProtocolHandler>, PT_ProtoTypesNum> _proto_handlers;
		std::vector<std::unique_ptr<ProtocolQueue>>	_inques;	int _seed_inque;
		std::vector<std::unique_ptr<ProtocolQueue>>	_outques;	int _seed_outque;
		std::vector<std::unique_ptr<PipeClient>>	_pipes;

	private:
		std::thread		_thread;
		bool			_quit;

	private:
		static void _on_connect(uv_stream_t* stream, int status);
		static void _on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
		static void _on_alloc_read_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
		static void _on_closed(uv_handle_t* handle);
	};

}







