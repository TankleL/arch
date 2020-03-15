#pragma once

#include "message.hpp"

class ServiceMessage : public Message
{
	UNCOPYABLE(ServiceMessage);

public:
	ServiceMessage();
	ServiceMessage(
		uint16_t conn_id,
		uint16_t ccf,
		archproto::ProtocolType proto,
		std::unique_ptr<archproto::IProtocolData>&& data,
		archsvc::PipeServer* pipe);

public:
	bool from(
		uint16_t conn_id,
		uint16_t ccf,
		archproto::ProtocolType proto,
		std::unique_ptr<archproto::IProtocolData>&& data,
		archsvc::PipeServer* pipe);

private:
	uint16_t				_conn_id;
	uint16_t				_ccf;
	archproto::ProtocolType	_proto;
	archsvc::PipeServer*	_pipe;
};



