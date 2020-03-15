#include "service-inst.hpp"
#include "service-msg.hpp"
#include "message-mgr.hpp"
#include "msg-registry.hpp"


using namespace archproto;
using namespace archsvc;

GameChannelServiceInstance::GameChannelServiceInstance(
	svc_id_t service_id,
	id_t id)
	: ServiceInstance(
		service_id,
		id,
		std::bind(
			&GameChannelServiceInstance::on_receive,
			this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4,
			std::placeholders::_5)
	)
{
	MsgRegistry::register_messages();
}


bool GameChannelServiceInstance::on_receive(
		uint16_t conn_id,
		uint16_t ccf,
		archproto::ProtocolType proto,
		std::unique_ptr<archproto::IProtocolData>&& data,
		archsvc::PipeServer& pipe)
{
	ServiceMessage msg(
		conn_id,
		ccf,
		proto,
		std::move(data),
		&pipe);

	MessageMgr::send_message(msg);
	return true;
}









