#pragma once

#include "archserver-prereq.hpp"
#include "uv.h"
#include "archmessagequeue.hpp"
#include "tcpconn.hpp"
#include "protocol.hpp"
#include "tcpsvc-workermgr.hpp"

namespace arch
{

	class UVTCPServer
	{
	public:
		UVTCPServer();
		~UVTCPServer();

	public:
		void run(
			IServiceProcessor* svc,
			const std::string& ipaddr = "0.0.0.0",
			int port = 5011,
			int backlog = 128,
			int core_number = 6);

	private:
		typedef struct WriteRequest {
			uv_write_t		req;
			uv_buf_t		buf;
			uv_handle_t*	link;
			std::string*	str;
			bool			close_conn;

			WriteRequest(std::string* buffer, uv_handle_t* hlink, bool close_connection = false);
			void dispose();
		} write_req_t;

	private:
		// for uv callbacks
		static void _on_connect(uv_stream_t *server, int status);
		static void _on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
		static void _on_close(uv_handle_t* handle);
		static void _on_alloc_buf(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
		static void _on_free_buf(const uv_buf_t* buf);
		static void _on_async_send(uv_async_t* handle);
		static void _after_write(uv_write_t *req, int status);

	private:
		void _switch_protocol(TCPConnection* conn);

	private:
		// for sending thread
		void _process_outnode(const ArchMessage& node);
		void _outing_listen_thread();
			
	private:
		TCPConnectionManager		_connmgr;
		uv_tcp_t					_uv_server;
		uv_loop_t*					_uv_loop;
		uv_async_t*					_uv_async_send;
		ArchMessageQueue*				_in_queue;
		ArchMessageQueue*				_out_queue;
		TCPServiceWorkerManager*	_workermgr;
		std::thread*				_othrd;
		ProtocolType				_psdestpt;
		bool						_abort_othrd;

		IProtocolProc*				_proto_procs[PT_ProtoTypesNum];
	};

}


