#include "tcpserver.hpp"
#include "connmap.hpp"

using namespace core;

TCPServer::TCPServer(
	const std::string& ipaddr,
	uint16_t port,
	int backlog)
	: _ipaddr(ipaddr)
	, _port(port)
	, _backlog(backlog)
	, _tcp_handle(*this, 0, {})
	, _thread(std::bind(&TCPServer::_work_thread, this))
	, _quit(false)
{}

TCPServer::~TCPServer()
{}

void TCPServer::wait()
{
	if(_thread.joinable())
		_thread.join();
}

void TCPServer::_work_thread()
{
	uv_loop_t	uvloop;
	uv_loop_init(&uvloop);

	uv_tcp_init(&uvloop, (uv_tcp_t*)&_tcp_handle);
	sockaddr_in addr;
	uv_ip4_addr(_ipaddr.c_str(), _port, &addr);
	uv_tcp_bind((uv_tcp_t*)&_tcp_handle, (const sockaddr*)& addr, 0);
	uv_listen((uv_stream_t*)& _tcp_handle, _backlog, _on_connect);

	uv_run(&uvloop, UV_RUN_DEFAULT);
}

void TCPServer::_on_connect(
	uv_stream_t* stream,
	int status)
{
	tcp_t& tcp_handle = (tcp_t&)* stream;

	if (status)
	{
		auto conn = tcp_handle.svr._connections.new_connection();
		auto connection = conn.lock();
		connection->set_stream(
			std::make_shared<tcp_t>(
				tcp_handle.svr,
				connection->get_id(),
				conn));

		if (!uv_accept(stream, (uv_stream_t*)&*(connection->get_stream().lock())))
		{
			uv_read_start(
				(uv_stream_t*)&*(connection->get_stream().lock()),
				_on_alloc_read_buf,
				_on_read);
		}
		else
		{
			tcp_handle.svr._close_connection(*connection);
		}
	}
}

void TCPServer::_on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	tcp_t& tcp_handle = (tcp_t&)* stream;

	auto& conn = tcp_handle.conn.lock();
	if (conn && nread)
	{

	}
	else
	{
		tcp_handle.svr._close_connection(*conn);
	}
}

void TCPServer::_on_alloc_read_buf(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	buf->base = new char[suggested_size];
	buf->len = (uint32_t)suggested_size;
}

void TCPServer::_on_closed(uv_handle_t* handle)
{
	tcp_t& tcp_handle = (tcp_t&)* handle;
	tcp_handle.svr._connections.delete_connection(tcp_handle.conn_id);
}

void TCPServer::_close_connection(const Connection<tcp_t>& conn)
{
	if (!conn.get_closing())
	{
		uv_close((uv_handle_t*) & (conn.get_stream().lock()), _on_closed);
	}
}











