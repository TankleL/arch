#include "service-inst.hpp"
#include "service-mgr.hpp"

using namespace svc;
using namespace core;

ServiceInstance::ServiceInstance(const core::IProtocolData::service_inst_id_t& id)
	: _pipecli(
		std::make_unique<core::PipeClient>(
			ServiceManager::get_ioques(id).inque,
			ServiceManager::get_ioques(id).outque))
	, _id(id)
{}

ServiceInstance::ServiceInstance(ServiceInstance&& rhs) noexcept
	: _pipecli(std::move(rhs._pipecli))
	, _id(rhs._id)
{}

ServiceInstance& ServiceInstance::operator=(ServiceInstance&& rhs) noexcept
{
	_id = rhs._id;
	return *this;
}

ServiceInstance::~ServiceInstance()
{}





