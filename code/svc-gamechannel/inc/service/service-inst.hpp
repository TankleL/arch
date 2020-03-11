#pragma once

#include "pre-req.hpp"

class GameChannelServiceInstance
	: public archsvc::ServiceInstance
{
public:
	GameChannelServiceInstance(
		svc_id_t service_id,
		id_t id);

private:
	bool on_receive(
		uint16_t conn_id,
		uint16_t ccf,
		archproto::ProtocolType proto,
		std::unique_ptr<archproto::IProtocolData>&& data,
		archsvc::PipeServer& pipe);

private:


};







