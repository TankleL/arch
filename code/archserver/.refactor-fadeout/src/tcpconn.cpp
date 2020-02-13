#include "archexception.hpp"
#include "tcpconn.hpp"

using namespace std;
using namespace arch;


/********************************************************************
*                          TCPConnection                            *
********************************************************************/

TCPConnection::TCPConnection(
	TCPServer* uvserver,
	uint64_t cid,
	uv_stream_t* hlink,
	ProtocolType iproto_type,
	ProtocolType oproto_type,
	uint32_t uid) noexcept
	: _uvserver(uvserver)
	, _cid(cid)
	, _uid(uid)
	, _hlink(hlink)
	, _protoobj(nullptr)
	, _iproto_type(iproto_type)
	, _oproto_type(oproto_type)
	, _closing(false)
{}

TCPConnection::~TCPConnection()
{
	if(_protoobj)	_protoobj->dispose();
}

TCPServer* TCPConnection::get_uvserver() const noexcept
{
	return _uvserver;
}

uint64_t TCPConnection::get_cid() const noexcept
{
	return _cid;
}

uv_stream_t* TCPConnection::get_hlink() const noexcept
{
	return _hlink;
}

IProtocolObject* TCPConnection::get_proto_obj() const noexcept
{
	return _protoobj;
}

ProtocolType TCPConnection::get_iproto_type() const noexcept
{
	return _iproto_type;
}

ProtocolType TCPConnection::get_oproto_type() const noexcept
{
	return _oproto_type;
}

uint32_t TCPConnection::get_uid() const noexcept
{
	return _uid;
}

bool TCPConnection::get_closing() const noexcept
{
	return _closing;
}

void TCPConnection::set_proto_obj(IProtocolObject* obj) noexcept
{
	_protoobj = obj;
}

void TCPConnection::set_iproto_type(ProtocolType type) noexcept
{
	_iproto_type = type;
}

void TCPConnection::set_oproto_type(ProtocolType type) noexcept
{
	_oproto_type = type;
}

void TCPConnection::set_closing(bool closing) noexcept
{
	_closing = closing;
}

/********************************************************************
*                       TCPConnectionManager                        *
********************************************************************/

TCPConnectionManager::TCPConnectionManager(TCPServer* uvserver) noexcept
	: _cidtop(0)
	, _uvserver(uvserver)
{}

TCPConnection* TCPConnectionManager::new_connection(uv_stream_t* hlink, ProtocolType itype, ProtocolType otype)
{
	return _alloc_conn(hlink, itype, otype);
}

TCPConnection* TCPConnectionManager::get_connection(uv_stream_t* hlink) noexcept
{
	return _find_conn(hlink);
}

TCPConnection* TCPConnectionManager::get_connection(uint64_t cid) noexcept
{
	return _find_conn(cid);
}

void TCPConnectionManager::del_connection(uv_stream_t* hlink) noexcept
{
	return _free_conn(hlink);
}

void TCPConnectionManager::del_connection(uint64_t cid) noexcept
{
	return _free_conn(cid);
}

TCPServer* TCPConnectionManager::get_server() const noexcept
{
	return _uvserver;
}

TCPConnection* TCPConnectionManager::_find_conn(uv_stream_t* hlink) const noexcept
{
	auto iter = _map_hlink2conn.find(hlink);
	if (iter != _map_hlink2conn.end())
	{
		return iter->second;
	}
	else
	{
		return nullptr;
	}
}

TCPConnection* TCPConnectionManager::_find_conn(uint64_t cid) const noexcept
{
	auto iter = _map_cid2conn.find(cid);
	if (iter != _map_cid2conn.end())
	{
		return iter->second;
	}
	else
	{
		return nullptr;
	}
}

TCPConnection* TCPConnectionManager::_alloc_conn(uv_stream_t* hlink, ProtocolType itype, ProtocolType otype)
{
	TCPConnection* conn = _find_conn(hlink);
	if (conn)
	{
		throw ArchException_ItemAlreadyExist();
	}

	try
	{
		conn = new TCPConnection(_uvserver, _cidtop++, hlink, itype, otype);
	}
	catch (const std::exception&)
	{
		throw ArchException_ItemAllocFailed();
	}

	try
	{
		_map_hlink2conn[hlink] = conn;
	}
	catch (const std::exception&)
	{
		delete conn;
		throw ArchException_ItemAllocFailed();
	}

	try
	{
		_map_cid2conn[conn->get_cid()] = conn;
	}
	catch (const std::exception&)
	{
		_map_hlink2conn.erase(hlink);
		delete conn;
		throw ArchException_ItemAllocFailed();
	}

	return conn;
}

void TCPConnectionManager::_free_conn(uv_stream_t* hlink) noexcept
{
	auto iter_hlink = _map_hlink2conn.find(hlink);
	if(iter_hlink != _map_hlink2conn.end())
	{ 
		auto conn = iter_hlink->second;
		_map_hlink2conn.erase(iter_hlink);

		assert(conn);
		_map_cid2conn.erase(conn->get_cid());

		delete conn;
	}
}

void TCPConnectionManager::_free_conn(uint64_t cid) noexcept
{
	auto iter_cid = _map_cid2conn.find(cid);
	if (iter_cid != _map_cid2conn.end())
	{
		auto conn = iter_cid->second;
		_map_cid2conn.erase(iter_cid);

		assert(conn);
		_map_hlink2conn.erase(conn->get_hlink());

		delete conn;
	}
}

