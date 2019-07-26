#include "archserver-prereq.hpp"
#include "protocols.hpp"

using namespace std;
using namespace arch;

ProtocolObjectHttp::ProtocolObjectHttp()
	: method(HMUnknown)
	, version(HVUnknown)
	, headers(new header_list_t())
	, path(new std::string())
	, status_code(0)
	, content_length(0)
	, content(new std::string())
	, status_msg(new std::string())
{}

ProtocolObjectHttp::ProtocolObjectHttp(ProtocolObjectHttp&& rhs) noexcept
	: method(rhs.method)
	, version(rhs.version)
	, headers(rhs.headers)
	, path(rhs.path)
	, status_code(rhs.status_code)
	, content_length(rhs.content_length)
	, content(rhs.content)
	, status_msg(rhs.status_msg)
{
	rhs.headers = nullptr;
	rhs.path = nullptr;
	rhs.content = nullptr;
	rhs.status_msg = nullptr;
}

ProtocolObjectHttp::~ProtocolObjectHttp()
{
	safe_delete(headers);
	safe_delete(path);
	safe_delete(content);
	safe_delete(status_msg);
}

ProtocolObjectHttp& ProtocolObjectHttp::operator=(ProtocolObjectHttp&& rhs) noexcept
{
	method			= rhs.method;
	version			= rhs.version;
	headers			= rhs.headers;		rhs.headers = nullptr;
	path			= rhs.path;			rhs.path = nullptr;
	status_code		= rhs.status_code;
	content_length	= rhs.content_length;
	content			= rhs.content;		rhs.content = nullptr;
	status_msg		= rhs.status_msg;	rhs.status_msg = nullptr;

	return *this;
}

IServiceDataObject& ProtocolObjectHttp::acquire(IServiceDataObject& src) noexcept
{
	ProtocolObjectHttp& obj = static_cast<ProtocolObjectHttp&>(src);
	headers			= obj.headers;		obj.headers = nullptr;
	path			= obj.path;			obj.path = nullptr;
	content			= obj.content;		obj.content = nullptr;
	status_msg		= obj.status_msg;	obj.status_msg = nullptr;

	return *this;
}

void ProtocolObjectHttp::dispose() noexcept
{
	delete this;
}





ProtocolObjectWebSocket::ProtocolObjectWebSocket()
{}

ProtocolObjectWebSocket::ProtocolObjectWebSocket(ProtocolObjectWebSocket&& rhs) noexcept
{}

ProtocolObjectWebSocket::~ProtocolObjectWebSocket()
{}

ProtocolObjectWebSocket& ProtocolObjectWebSocket::operator=(ProtocolObjectWebSocket&& rhs) noexcept
{
	return *this;
}

IServiceDataObject& ProtocolObjectWebSocket::acquire(IServiceDataObject& src) noexcept
{
	ProtocolObjectWebSocket& obj = static_cast<ProtocolObjectWebSocket&>(src);
	return *this;
}

void ProtocolObjectWebSocket::dispose() noexcept
{
	delete this;
}

