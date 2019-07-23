#pragma once

#include "archserver-prereq.hpp"
#include "uv.h"
#include "tcpdata-queue-node.hpp"
#include "buffer.hpp"
#include "tcpservice-protocols.hpp"
#include "protocol.hpp"


namespace arch
{
	class UVTCPServer;
	class TCPConnectionManager;

	class TCPConnection
	{
		friend TCPConnectionManager;
	public:
		virtual ~TCPConnection();

	public:
		UVTCPServer*		get_uvserver() const noexcept;
		uint64_t			get_cid() const noexcept;
		uv_stream_t*		get_hlink() const noexcept;
		IProtocolObject*	get_proto_obj() const noexcept;
		ProtocolType		get_iproto_type() const noexcept;
		ProtocolType		get_oproto_type() const noexcept;
		uint32_t			get_uid() const noexcept;

		void set_proto_obj(IProtocolObject* obj) noexcept;
		void set_iproto_type(ProtocolType type) noexcept;
		void set_oproto_type(ProtocolType type) noexcept;

	protected:
		TCPConnection(
			UVTCPServer* uvserver,
			uint64_t cid,
			uv_stream_t* hlink,
			ProtocolType iproto_type = PT_Http,
			ProtocolType oproto_type = PT_Http,
			uint32_t uid = 0) noexcept;

	protected:
		uint64_t			_cid;			// correlation id
		uint32_t			_uid;			// user id
		uv_stream_t*		_hlink;			// link handle
		UVTCPServer*		_uvserver;		// uv server
		IProtocolObject*	_protoobj;		// protocol cache object
		ProtocolType		_iproto_type;	// input protocol type
		ProtocolType		_oproto_type;	// output protocol type
	};

	class TCPConnectionManager
	{
	public:
		TCPConnectionManager(UVTCPServer* uvserver) noexcept;

	public:
		TCPConnection* new_connection(uv_stream_t* hlink, ProtocolType itype, ProtocolType otype);
		TCPConnection* get_connection(uv_stream_t* hlink) noexcept;
		TCPConnection* get_connection(uint64_t cid) noexcept;
		void del_connection(uv_stream_t* hlink) noexcept;
		void del_connection(uint64_t cid) noexcept;

		UVTCPServer* get_server() const noexcept;

	protected:
		TCPConnection* _alloc_conn(uv_stream_t* hlink, ProtocolType itype, ProtocolType otype);
		TCPConnection* _find_conn(uv_stream_t* hlink) const noexcept;
		TCPConnection* _find_conn(uint64_t cid) const noexcept;
		void _free_conn(uv_stream_t* hlink) noexcept;
		void _free_conn(uint64_t cid) noexcept;

	protected:
		typedef std::unordered_map<uv_stream_t*, TCPConnection*>	MapHlink2Conn;
		typedef std::unordered_map<uint64_t, TCPConnection*>		MapCid2Conn;

	protected:
		MapHlink2Conn		_map_hlink2conn;
		MapCid2Conn			_map_cid2conn;

		uint64_t			_cidtop;
		UVTCPServer*		_uvserver;
	};
}

