#include "tcpserver.hpp"
#include "connmap.hpp"
#include "protocol.hpp"
#include "protocol-arch.hpp"
#include "service-mgr.hpp"
#include "ccf.hpp"

using namespace core;
using namespace archproto;

TCPServer::TCPServer(
	const std::string& ipaddr,
	uint16_t port,
	int backlog)
	: _ipaddr(ipaddr)
	, _port(port)
	, _backlog(backlog)
	, _uvloop({})
	, _tcp_handle(*this, 0, {})
	, _tm_write(*this)
	, _thread(std::bind(&TCPServer::_work_thread, this))
	, _quit(false)
	, _proto_handlers{
		/*PT_Http*/ nullptr,
		/*PT_WebSocket */ nullptr,
		/*PT_Arch*/ std::make_unique<ArchProtocol>()}
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
	uv_timer_init(&_uvloop, &_tm_write);
	sockaddr_in addr;
	uv_ip4_addr(_ipaddr.c_str(), _port, &addr);
	uv_tcp_bind((uv_tcp_t*)&_tcp_handle, (const sockaddr*)& addr, 0);
	uv_listen((uv_stream_t*)& _tcp_handle, _backlog, _on_connect);
	uv_timer_start(&_tm_write, _on_write_timer, 1, 0);

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
			_close_connection(conn->get_stream());
		}
	}
}

void TCPServer::_on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	tcp_t& tcp_handle = (tcp_t&)* stream;

	auto conn = tcp_handle.conn;
	if (conn && nread > 0)
	{
		assert(conn->get_tempproto() < PT_ProtoTypesNum);

		bool goon = true;
		size_t offset = 0;
		while(goon)
		{
			tcp_handle.svr._ensure_protocol_data(*conn);
			IProtocolData& tmpdata = conn->get_tempdata();

			IProtocolHandler* phdl = tcp_handle.svr._get_protocol_handler(conn->get_tempproto());

			size_t	procbytes = 0;
			IProtocolHandler::ProtoProcRet proc_ret = phdl->des_sock_stream(
				tmpdata,
				(uint8_t*)(buf->base + offset),
				nread - offset,
				procbytes);
			offset += procbytes;
			switch (proc_ret)
			{
			case IProtocolHandler::PPR_AGAIN:
				break;

			case IProtocolHandler::PPR_PULSE:
			{
				ProtocolType prot_switchto = PT_Unknown;
				if (!phdl->chk_sock_protoswitch(prot_switchto, tmpdata))
				{
					ProtocolQueue::node_t node(
						conn->acquire_tempdata(),
						conn->get_id(),
						0,
						conn->get_tempproto());

					if(!svc::ServiceMgr::dispatch_protocol_data(std::move(node)))
					{
						goon = false;
						_close_connection(&tcp_handle);
					}
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
				_close_connection(&tcp_handle);
				break;
			}

			goon = goon && (offset < (size_t)nread);
		}
	}
	else if(nullptr == conn || nread < 0)
	{
		_close_connection(&tcp_handle);
	}

	delete buf->base;
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

void TCPServer::_on_write_timer(uv_timer_t* handle)
{
	uvtimer_t& tm_handle = (uvtimer_t&)* handle;
	auto& outnodes = tm_handle.svr._tmp_outnodes;
	auto& connmap = tm_handle.svr._connections;

	svc::ServiceMgr::pull_protocol_data(outnodes);

	for (const auto& node : outnodes)
	{
		auto conn = connmap.get_connection(node.conn_id);
		if (conn)
		{
			if (node.data->length() > 0)
			{
				IProtocolHandler* phdl = tm_handle.svr._get_protocol_handler(node.proto);
				std::vector<uint8_t> stream;

				if (phdl->ser_sock_stream(stream, *node.data))
				{
					write_req_t* req = new write_req_t(
						std::move(stream),
						node.ccf);
					uv_write(
						req,
						(uv_stream_t*)conn->get_stream(),
						&req->buf,
						1,
						_on_written);
				}
				else
				{ // failed to generate response stream data
					_close_connection(conn->get_stream());
				}
			}
			else
			{ // empty response data.
				if (ccf_check(node.ccf, CCF_Close))
				{
					_close_connection(conn->get_stream());
				}
			}
		}
	}
	outnodes.clear();

	uv_timer_start(&tm_handle, _on_write_timer, 1, 0);
}

void TCPServer::_on_written(uv_write_t* req, int status)
{
	write_req_t* wrt_req = (write_req_t*)req;
	if (ccf_check(wrt_req->ccf, CCF_Close))
	{
		_close_connection((tcp_t*)wrt_req->handle);
	}

	delete wrt_req;
}

void TCPServer::_close_connection(tcp_t* handle)
{
	if (!uv_is_closing((uv_handle_t*)handle))
	{
		uv_close((uv_handle_t*)handle, _on_closed);
	}
}

void TCPServer::_ensure_protocol_data(Connection<tcp_t>& conn)
{
	if (!conn.has_tempdata())
	{
		switch (conn.get_tempproto())
		{
		case PT_Arch:
			conn.set_tempdata(
				std::make_unique<ArchProtocolData>());
			break;

		default:
			assert(false);
		}
	}
}

IProtocolHandler* TCPServer::_get_protocol_handler(ProtocolType ptype)
{
	return _proto_handlers[ptype].get();
}

void TCPServer::_switch_protocol(Connection<tcp_t>& conn)
{
	// TODO: need implementation
}












