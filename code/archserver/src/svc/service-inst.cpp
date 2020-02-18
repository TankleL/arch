#include "service-inst.hpp"
#include "service-mgr.hpp"

using namespace svc;
using namespace core;

ServiceInstance::ServiceInstance(
		const Service& service,
		const core::IProtocolData::service_inst_id_t& id,
		const std::shared_ptr<core::ProtocolQueue>& inque,
		const std::shared_ptr<core::ProtocolQueue>& outque)
	: _pipecli(
		std::make_unique<core::PipeClient>(
			inque, outque))
	, _id(id)
	, _process(0)
{
	std::string appname = service._path + "/" + service._assembly;
	_process = osys::create_process(
		appname,
		"s1",
		service._workingdir);
}

ServiceInstance::ServiceInstance(ServiceInstance&& rhs) noexcept
	: _pipecli(std::move(rhs._pipecli))
	, _id(rhs._id)
	, _process(std::move(rhs._process))
{}

ServiceInstance& ServiceInstance::operator=(ServiceInstance&& rhs) noexcept
{
	_id = rhs._id;
	return *this;
}

ServiceInstance::~ServiceInstance()
{}





