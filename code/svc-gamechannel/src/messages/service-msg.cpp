#include "service-msg.hpp"

using namespace archproto;
using namespace archsvc;

ServiceMessage::ServiceMessage()
	: Message()
	, _conn_id(0)
	, _ccf(0)
	, _proto(PT_Unknown)
	, _pipe(nullptr)
{}

ServiceMessage::ServiceMessage(
		uint16_t conn_id,
		uint16_t ccf,
		archproto::ProtocolType proto,
		std::unique_ptr<archproto::IProtocolData>&& data,
		archsvc::PipeServer* pipe)
	: ServiceMessage()
{
	if (!from(
			conn_id,
			ccf,
			proto,
			std::move(data),
			pipe))
	{
		throw std::runtime_error("bad protocol data");
	}
}

bool ServiceMessage::from(
		uint16_t conn_id,
		uint16_t ccf,
		archproto::ProtocolType proto,
		std::unique_ptr<archproto::IProtocolData>&& data,
		archsvc::PipeServer* pipe)
{
	_conn_id = conn_id;
	_ccf = ccf;
	_proto = proto;
	_pipe = pipe;

	return Message::from(std::move(data));
}


