#include "asvc-service-instance.hpp"
#include <sstream>

using namespace archsvc;

archsvc::ServiceInstance::ServiceInstance(
	const svc_id_t& svc_id,
	const id_t& id,
	const PipeServer::receiver_t& receiver)
	: _svc_id(svc_id)
	, _id(id)
	, _receiver(receiver)
{}

archsvc::ServiceInstance::~ServiceInstance()
{}

void archsvc::ServiceInstance::run()
{
	_pipesvr = std::move(
		std::make_unique<PipeServer>(
			_pipename(),
			_receiver));

	_pipesvr->wait();
}

std::string archsvc::ServiceInstance::_pipename() const
{
	std::string r1, r2;

	std::stringstream ss1;
	ss1 << _svc_id;
	ss1 >> r1;

	std::stringstream ss2;
	ss2 << _id;
	ss2 >> r2;

	return "asvc_data_pipe_" + r1 + r2;
}








