#include "asvc-service-instance.hpp"
#include <sstream>

using namespace archsvc;

archsvc::ServiceInstance::ServiceInstance(const id_t& id)
	: _id(id)
{}

archsvc::ServiceInstance::~ServiceInstance()
{}

void archsvc::ServiceInstance::run()
{
	std::ostringstream oss_pipename;
	oss_pipename << "asvc_data_pipe_" << _id;

	_pipesvr = std::move(
		std::make_unique<PipeServer>(oss_pipename.str()));

	_pipesvr->wait();
}








