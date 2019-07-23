#pragma once

#include "archserver-prereq.hpp"
#include "buffer.hpp"
#include "tcpservice.hpp"

namespace arch
{
	enum ProtocolType : int
	{
		PT_Http = 0,
		PT_WebSocket = 1,

		// < ----------- insert new types here.

		PT_ProtoTypesNum,
		PT_Unknown = -1
	};

	enum HttpMethod : int
	{
		HMUnknown,
		HMGET,
		HMHEAD,
		HMPOST,
		HMPUT
	};

	enum HttpVersion : int
	{
		HVUnknown,
		HVV1,
		HVV1_1,
		HVV2
	};

	class IProtocolObject
		: public IServiceDataObject
	{
	public:
		virtual ProtocolType	get_protocol_type() const noexcept = 0;
	};

	class ProtocolObjectHttp
		: public IProtocolObject
	{
	public:
		ProtocolObjectHttp(const ProtocolObjectHttp& rhs) = delete;
		ProtocolObjectHttp& operator=(const ProtocolObjectHttp& rhs) = delete;

	public:
		ProtocolObjectHttp();
		ProtocolObjectHttp(ProtocolObjectHttp&& rhs) noexcept;
		virtual ~ProtocolObjectHttp();

		ProtocolObjectHttp& operator=(ProtocolObjectHttp&& rhs) noexcept;

	public:
		virtual ProtocolType	get_protocol_type() const noexcept override	{ return PT_Http; }
		IServiceDataObject&		acquire(IServiceDataObject& src) noexcept override;
		void					dispose() noexcept override;

	public:
		typedef std::map<std::string, std::string>	header_list_t;

	public:
		HttpMethod			method;
		HttpVersion			version;
		header_list_t*		headers;
		std::string*		path;
		int					status_code;
		index_t				content_length;
		std::string*		content;
		std::string*		status_msg;
	};

	class ProtocolObjectWebSocket
		: public IProtocolObject
	{
	public:
		ProtocolObjectWebSocket(const ProtocolObjectWebSocket& rhs) = delete;
		ProtocolObjectWebSocket& operator=(const ProtocolObjectWebSocket& rhs) = delete;

	public:
		ProtocolObjectWebSocket();
		ProtocolObjectWebSocket(ProtocolObjectWebSocket&& rhs) noexcept;
		virtual ~ProtocolObjectWebSocket();

		ProtocolObjectWebSocket& operator=(ProtocolObjectWebSocket&& rhs) noexcept;

	public:
		virtual ProtocolType	get_protocol_type() const noexcept override { return PT_WebSocket; }
		IServiceDataObject&		acquire(IServiceDataObject& src) noexcept override;
		void					dispose() noexcept override;

	public:
		ArchBuffer	buffer;
	};
}


