#pragma once

#include <map>
#include "service-dataobj.hpp"

namespace arch
{
	/******************************************************************/
	/*                    ------ Definition ------                    */
	/*                          ProtocolType                          */
	/******************************************************************/

	enum ProtocolType : int
	{
		PT_Http = 0,
		PT_WebSocket = 1,

		// < ----------- insert new types here.

		PT_ProtoTypesNum,
		PT_Unknown = -1
	};

	/******************************************************************/
	/*                   ------ Declaration ------                    */
	/*                        IProtocolObject                         */
	/******************************************************************/

	class IProtocolObject
		: public IServiceDataObject
	{
	public:
		virtual ProtocolType	get_protocol_type() const noexcept = 0;
	};

	/******************************************************************/
	/*                   ------ Declaration ------                    */
	/*                      Protocol HTTP Object                      */
	/******************************************************************/

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
		header_list_t		headers;
		std::string			path;
		int					status_code;
		int					content_length;
		std::string			content;
		std::string			status_msg;
	};

	/******************************************************************/
	/*                   ------ Declaration ------                    */
	/*                   Protocol WebSocket Object                    */
	/******************************************************************/

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
	};


	/******************************************************************/
	/*                    ------ Definition ------                    */
	/*                       Protocol HTTP Object                     */
	/******************************************************************/
	inline ProtocolObjectHttp::ProtocolObjectHttp()
		: method(HMUnknown)
		, version(HVUnknown)
		, status_code(0)
		, content_length(0)
	{}

	inline ProtocolObjectHttp::ProtocolObjectHttp(ProtocolObjectHttp&& rhs) noexcept
		: method(rhs.method)
		, version(rhs.version)
		, headers(std::move(rhs.headers))
		, path(std::move(rhs.path))
		, status_code(rhs.status_code)
		, content_length(rhs.content_length)
		, content(std::move(rhs.content))
		, status_msg(std::move(rhs.status_msg))
	{}

	inline ProtocolObjectHttp::~ProtocolObjectHttp()
	{}

	inline ProtocolObjectHttp& ProtocolObjectHttp::operator=(ProtocolObjectHttp&& rhs) noexcept
	{
		method = rhs.method;
		version = rhs.version;
		headers = std::move(rhs.headers);
		path = std::move(rhs.path);
		status_code = rhs.status_code;
		content_length = rhs.content_length;
		content = std::move(rhs.content);
		status_msg = std::move(rhs.status_msg);

		return *this;
	}

	inline IServiceDataObject& ProtocolObjectHttp::acquire(IServiceDataObject& src) noexcept
	{
		ProtocolObjectHttp& obj = static_cast<ProtocolObjectHttp&>(src);
		method = obj.method;
		version = obj.version;
		status_code = obj.status_code;
		content_length = obj.content_length;

		headers = std::move(obj.headers);
		path = std::move(obj.path);
		content = std::move(obj.content);
		status_msg = std::move(obj.status_msg);

		return *this;
	}

	inline void ProtocolObjectHttp::dispose() noexcept
	{
		delete this;
	}



	/******************************************************************/
	/*                    ------ Definition ------                    */
	/*                   Protocol WebSocket Object                    */
	/******************************************************************/

	inline ProtocolObjectWebSocket::ProtocolObjectWebSocket()
	{}

	inline ProtocolObjectWebSocket::ProtocolObjectWebSocket(ProtocolObjectWebSocket&& rhs) noexcept
	{}

	inline ProtocolObjectWebSocket::~ProtocolObjectWebSocket()
	{}

	inline ProtocolObjectWebSocket& ProtocolObjectWebSocket::operator=(ProtocolObjectWebSocket&& rhs) noexcept
	{
		return *this;
	}

	inline IServiceDataObject& ProtocolObjectWebSocket::acquire(IServiceDataObject& src) noexcept
	{
		ProtocolObjectWebSocket& obj = static_cast<ProtocolObjectWebSocket&>(src);
		return *this;
	}

	inline void ProtocolObjectWebSocket::dispose() noexcept
	{
		delete this;
	}
}


