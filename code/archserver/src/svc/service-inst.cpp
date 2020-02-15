#include "service-inst.hpp"

using namespace svc;
using namespace core;

ServiceInstance::ServiceInstance(const service_inst_id_t& id)
	: _id(id)
{}

ServiceInstance::ServiceInstance(ServiceInstance&& rhs) noexcept
	: _id(rhs._id)
{}

ServiceInstance& ServiceInstance::operator=(ServiceInstance&& rhs) noexcept
{
	_id = rhs._id;
	return *this;
}

ServiceInstance::~ServiceInstance()
{}





