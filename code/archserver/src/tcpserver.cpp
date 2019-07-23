#include "sha1.hpp"
#include "tcpserver.hpp"
#include "protocol-http.hpp"

using namespace std;
using namespace arch;

UVTCPServer::WriteRequest::WriteRequest(std::string* buffer, uv_handle_t* hlink, bool close_connection)
	: link(hlink)
	, str(buffer)
	, close_conn(close_connection)
{
	memset(&req, 0, sizeof(req));
	buf.base = (char*)buffer->data();
	buf.len = (uint32_t)buffer->size();
}

void UVTCPServer::WriteRequest::dispose()
{
	safe_delete(str);
	buf.base = nullptr;
	buf.len = 0;
	delete this;
}

UVTCPServer::UVTCPServer()
	: _connmgr(this)
	, _uv_loop(nullptr)
	, _in_queue(nullptr)
	, _out_queue(nullptr)
	, _workermgr(nullptr)
	, _othrd(nullptr)
	, _abort_othrd(false)
{
	// intialize uv_server handle
	memset(&_uv_server, 0, sizeof(_uv_server));

	// set up data queues
	_in_queue = new TCPDataQueue();
	_out_queue = new TCPDataQueue();
	
	// set up uv loop
	_uv_loop = new uv_loop_t;
	uv_loop_init(_uv_loop);

	// set up async calls
	_uv_async_send = new uv_async_t;
	_uv_async_send->data = this;
	uv_async_init(_uv_loop, _uv_async_send, _on_async_send);

	// set up protocol procs
	for (int proto_type = 0; proto_type < PT_ProtoTypesNum; ++proto_type)
	{
		switch (proto_type)
		{
		case PT_Http:
			_proto_procs[proto_type] = new ProtoProcHttp();
			break;

		default:
			_proto_procs[proto_type] = nullptr;
		}
	}

	_othrd = new std::thread(std::bind(&UVTCPServer::_outing_listen_thread, this));
}

UVTCPServer::~UVTCPServer()
{
	_workermgr->abort();
	_workermgr->wait();
	safe_delete(_workermgr);

	_abort_othrd = true;
	_othrd->join();
	safe_delete(_othrd);

	for (int proto_type = 0; proto_type < PT_ProtoTypesNum; ++proto_type)
	{
		if (_proto_procs[proto_type]) delete _proto_procs[proto_type];
	}

	safe_delete(_uv_loop);
	safe_delete(_in_queue);
	safe_delete(_out_queue);
}

void UVTCPServer::run(
	IServiceProcessor* svc,
	const std::string& ipaddr,
	int port,
	int backlog,
	int core_number
)
{
	uv_tcp_init(_uv_loop, &_uv_server);
	sockaddr_in	addr;
	uv_ip4_addr(ipaddr.c_str(), port, &addr);
	uv_tcp_bind(&_uv_server, (const sockaddr*)&addr, 0);

	_workermgr = new TCPServiceWorkerManager(_in_queue, _out_queue, svc, core_number);
	_workermgr->start();

	_uv_server.data = this;
	int res = uv_listen((uv_stream_t*)& _uv_server, backlog, *_on_connect);

	uv_run(_uv_loop, UV_RUN_DEFAULT);
}

void UVTCPServer::_on_connect(uv_stream_t *server, int status)
{
	UVTCPServer*	svr_inst = static_cast<UVTCPServer*>(server->data);

	if (status >= 0)
	{
		uv_tcp_t *client = new uv_tcp_t;
		uv_tcp_init(svr_inst->_uv_loop, client);

		/*svr_inst->_proto_procs[PT_Http],*/

		TCPConnection* conn = svr_inst->_connmgr.new_connection(
			(uv_stream_t*)client,
			PT_Http,	// Use http as defualt protocol
			PT_Http);	// Use http as defualt protocol
		client->data = conn;

		if (!uv_accept(server, (uv_stream_t*)client))
		{
			uv_read_start(
				(uv_stream_t*)client,
				_on_alloc_buf,
				_on_read);
		}
		else
		{
			uv_close((uv_handle_t*)client, _on_close);
		}
	}
	else
	{
		// TODO: report error
	}
}

void UVTCPServer::_on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
	if (nread >= 0)
	{
		TCPConnection*	conn = static_cast<TCPConnection*>(client->data);
		UVTCPServer* svr_inst = conn->get_uvserver();

		assert(conn);
		assert(conn->get_iproto_type() < PT_ProtoTypesNum);

		bool goon = true;
		if (nullptr == conn->get_proto_obj())
		{
			switch (conn->get_iproto_type())
			{
			case PT_Http:
				conn->set_proto_obj(new Internal_ProtoObjectHttp());
				break;

			default:
				goon = false;
				uv_close((uv_handle_t*)client, _on_close);
			}
		}

		IProtocolProc* iproc = svr_inst->_proto_procs[conn->get_iproto_type()];

		if (goon)
		{
			ProtoProcRet proc_ret = iproc->proc_istrm(*conn->get_proto_obj(), buf, nread);
			switch (proc_ret)
			{
			case PPR_AGAIN:
				break;

			case PPR_PULSE:
			{
				if (!iproc->proc_check_switch(svr_inst->_psdestpt, *conn->get_proto_obj()))
				{
					TCPDataQueueNode* inode = new TCPDataQueueNode(conn->get_proto_obj(), conn->get_hlink(), conn->get_uid());
					conn->set_proto_obj(nullptr);
					conn->get_uvserver()->_in_queue->push(inode);
				}
				else
				{
					svr_inst->_switch_protocol(conn);
				}
			}
				break;

			case PPR_ERROR:
			case PPR_CLOSE:
			default:
				uv_close((uv_handle_t*)client, _on_close);
				break;
			}
		}
	}
	else if (nread < 0)
	{
		if (nread != UV_EOF)
			printf("Read error %s\n", uv_err_name((int)nread));
		uv_close((uv_handle_t*)client, _on_close);
	}

	_on_free_buf(buf);
}

void UVTCPServer::_on_close(uv_handle_t* handle)
{
	TCPConnection* conn = static_cast<TCPConnection*>(handle->data);
	assert(conn);
	
	UVTCPServer* svr_inst = conn->get_uvserver();
	assert(svr_inst);

	svr_inst->_connmgr.del_connection(conn->get_hlink());

	delete handle;
	// from now on, conn has been released, don not use it any longer!!
}

void UVTCPServer::_on_alloc_buf(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	buf->base = new char[suggested_size];
	buf->len = (uint32_t)suggested_size;
}

void UVTCPServer::_on_free_buf(const uv_buf_t* buf)
{
	delete[] buf->base;
}

void UVTCPServer::_outing_listen_thread()
{
	while (!_abort_othrd)
	{
		if (_out_queue->size() > 0)
		{
			uv_async_send(_uv_async_send);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
}

void UVTCPServer::_on_async_send(uv_async_t* handle)
{
	UVTCPServer* svr_inst = static_cast<UVTCPServer*>(handle->data);
	TCPDataQueueNode* onode = nullptr;
	while (svr_inst->_out_queue->pop(&onode))
	{
		svr_inst->_process_outnode(*onode);
		delete onode;
	}
}

void UVTCPServer::_process_outnode(const TCPDataQueueNode& node)
{
#ifdef _DEBUG
	IProtocolObject* obj = dynamic_cast<IProtocolObject*>(node.get_data_object());
	assert(obj);
#else
	IProtocolObject* obj = static_cast<IProtocolObject*>(node.get_data_object());
#endif // _DEBUG


	ProtocolType otype = obj->get_protocol_type();
	assert(otype < PT_ProtoTypesNum);

	IProtocolProc* oproc = _proto_procs[otype];
	TCPConnection* conn = _connmgr.get_connection((uv_stream_t*)node.get_hlink());
	if (conn)
	{
		if (CCT_Close_Immediate != node.get_conn_ctrl_type())
		{
			try
			{
				std::string* obuffer = new std::string();
				oproc->proc_ostrm(*obuffer, *obj);
				write_req_t* req = new write_req_t(obuffer, (uv_handle_t*)node.get_hlink(), node.get_conn_ctrl_type());
				uv_write(
					(uv_write_t*)req,
					(uv_stream_t*)node.get_hlink(),
					&req->buf,
					1,
					_after_write
				);
			}
			catch (std::exception&)
			{
				//TODO: log the error.

				uv_close((uv_handle_t*)conn->get_hlink(), _on_close);
			}
		}
		else
		{
			uv_close((uv_handle_t*)conn->get_hlink(), _on_close);
		}
	}
}

void UVTCPServer::_after_write(uv_write_t *req, int status)
{
	assert(req);

	if (status)
	{
		// TODO: report error.
	}

	write_req_t* wr = (write_req_t*)req;
	if (CCT_Close_AfterSend == wr->close_conn)
	{
		uv_close(wr->link, _on_close);
	}
	wr->dispose();
}

void UVTCPServer::_switch_protocol(TCPConnection* conn)
{
	switch (_psdestpt)
	{
	case PT_WebSocket:
	{
		IProtocolObject* pobj = conn->get_proto_obj();
		ProtocolType pt = pobj->get_protocol_type();
		
		switch (pt)
		{
		case PT_Http:
		{
			ProtocolObjectHttp* obj = static_cast<ProtocolObjectHttp*>(pobj);

			// calculate ssa
			std::string ssa_str = obj->headers->at("Sec-WebSocket-Key") + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
			SHA1 sha1;
			sha1.update(ssa_str);
			ssa_str = sha1.final_base64();

			ProtocolObjectHttp* oobj = new ProtocolObjectHttp();
			oobj->status_code = 101;
			oobj->status_msg->assign("Switching Protocols");
			oobj->version = HVV1_1;
			oobj->path->assign("/");
			oobj->headers->insert(std::make_pair("Upgrade", "websocket"));
			oobj->headers->insert(std::make_pair("Connection", "Upgrade"));
			oobj->headers->insert(std::make_pair("Sec-WebSocket-Accept", ssa_str));

			TCPDataQueueNode* onode = new TCPDataQueueNode(oobj, conn->get_hlink(), conn->get_uid());
			conn->get_uvserver()->_out_queue->push(onode);

			obj->dispose();
			conn->set_proto_obj(nullptr);
			conn->set_iproto_type(PT_WebSocket);
		}
			break;

		default:
			TCPDataQueueNode* onode = new TCPDataQueueNode(conn->get_proto_obj(), conn->get_hlink(), conn->get_uid(), CCT_Close_Immediate);
			conn->set_proto_obj(nullptr);
			conn->get_uvserver()->_out_queue->push(onode);
		}
	}
		break;

	default:
		// unknown protocol, close connection
	{
		TCPDataQueueNode* onode = new TCPDataQueueNode(conn->get_proto_obj(), conn->get_hlink(), conn->get_uid(), CCT_Close_Immediate);
		conn->set_proto_obj(nullptr);
		conn->get_uvserver()->_out_queue->push(onode);
	}
	}
}


