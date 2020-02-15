#include "tcpserver.hpp"
#include "connmap.hpp"
#include "protocol.hpp"
#include "protocol-arch.hpp"

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
	, _proto_handlers{
		/*PT_Http*/ nullptr,
		/*PT_WebSocket */ nullptr,
		/*PT_Arch*/ std::make_unique<ipro::protocol_arch::ArchProtocol>()}
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
	uv_loop_init(&_uvloop);

	uv_tcp_init(&_uvloop, (uv_tcp_t*)&_tcp_handle);
	sockaddr_in addr;
	uv_ip4_addr(_ipaddr.c_str(), _port, &addr);
	uv_tcp_bind((uv_tcp_t*)&_tcp_handle, (const sockaddr*)& addr, 0);
	uv_listen((uv_stream_t*)& _tcp_handle, _backlog, _on_connect);

	uv_run(&_uvloop, UV_RUN_DEFAULT);
}

void TCPServer::_on_connect(
	uv_stream_t* stream,
	int status)
{
	tcp_t& tcp_handle = (tcp_t&)* stream;

	if (status >= 0)
	{
		auto conn = tcp_handle.svr._connections.new_connection(PT_Arch);
		conn->set_stream(
			std::make_unique<tcp_t>(
				tcp_handle.svr,
				conn->get_id(),
				conn));
		uv_tcp_init(&tcp_handle.svr._uvloop, conn->get_stream());

		if (!uv_accept(stream, (uv_stream_t*)conn->get_stream()))
		{
			uv_read_start(
				(uv_stream_t*)conn->get_stream(),
				_on_alloc_read_buf,
				_on_read);
		}
		else
		{
			tcp_handle.svr._close_connection(*conn);
		}
	}
}

void TCPServer::_on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	tcp_t& tcp_handle = (tcp_t&)* stream;

	auto& conn = tcp_handle.conn;
	if (conn && nread >= 0)
	{
		assert(conn->get_iapp_protocol() < PT_ProtoTypesNum);

		bool goon = true;
		size_t offset = 0;
		while(goon)
		{
			if (!tcp_handle.svr._ensure_protocol_data(*conn)) break;

			IProtocolHandler* phdl = tcp_handle.svr._get_protocol_handler(conn->get_iapp_protocol());

			size_t	procbytes = 0;
			IProtocolHandler::ProtoProcRet proc_ret = phdl->proc_istrm(
				*(conn->get_app_protocol_data()),
				(uint8_t*)(buf->base + offset),
				nread,
				procbytes);
			offset += procbytes;
			switch (proc_ret)
			{
			case IProtocolHandler::PPR_AGAIN:
				break;

			case IProtocolHandler::PPR_PULSE:
			{
				ProtocolType prot_switchto = PT_Unknown;
				if (!phdl->proc_check_switch(prot_switchto, *(conn->get_app_protocol_data())))
				{
					//ArchMessage inode(conn->get_proto_obj(), conn->get_hlink(), conn->get_uid());
					//conn->set_proto_obj(nullptr);
					//conn->get_uvserver()->_in_queue->push(std::move(inode));
				}
				else
				{
					tcp_handle.svr._switch_protocol(*conn);
				}
			}
			break;

			case IProtocolHandler::PPR_ERROR:
			case IProtocolHandler::PPR_CLOSE:
			default:
				tcp_handle.svr._close_connection(*conn);
				break;
			}

			goon = offset < (size_t)nread;
		}
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
		uv_close((uv_handle_t*) conn.get_stream(), _on_closed);
	}
}

bool TCPServer::_ensure_protocol_data(Connection<tcp_t>& conn)
{
	bool succ = true;
	if (nullptr == conn.get_app_protocol_data())
	{
		switch (conn.get_iapp_protocol())
		{
		case PT_Arch:
			conn.set_app_protocol_data(
				std::make_unique<ipro::protocol_arch::ArchProtocolData>());
			break;

		default:
			succ = false;
			_close_connection(conn);
		}
	}
	return succ;
}

IProtocolHandler* TCPServer::_get_protocol_handler(ProtocolType ptype)
{
	return _proto_handlers[ptype].get();
}

void TCPServer::_switch_protocol(Connection<tcp_t>& conn)
{
	// TODO: need implementation
}












