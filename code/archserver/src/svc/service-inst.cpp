#include "service-inst.hpp"
#include "service-mgr.hpp"

using namespace svc;
using namespace core;

ServiceInstance::ServiceInstance(
	const Service& service,
	const core::IProtocolData::service_inst_id_t& id,
	const std::shared_ptr<core::ProtocolQueue>& inque,
	const std::shared_ptr<core::ProtocolQueue>& outque)
	: _id(id)
{
	std::ostringstream appname;
	appname << service._path << "/" << service._assembly;
	std::ostringstream oss_cmdline;
	oss_cmdline << "-p" << id;
	_process = osys::create_process(
		appname.str(),
		oss_cmdline.str(),
		service._workingdir);

	std::ostringstream oss_pipename;
	oss_pipename << "asvc_data_pipe_" << id;
	_pipecli = std::make_unique<PipeClient>(
		oss_pipename.str(),
		inque,
		outque);
}

ServiceInstance::ServiceInstance(ServiceInstance&& rhs) noexcept
	: _pipecli(std::move(rhs._pipecli))
	, _id(rhs._id)
	, _process(std::move(rhs._process))
{}

ServiceInstance& ServiceInstance::operator=(ServiceInstance&& rhs) noexcept
{
	_pipecli = std::move(rhs._pipecli);
	_id = rhs._id;
	_process = std::move(rhs._process);
	return *this;
}

ServiceInstance::~ServiceInstance()
{}

void ServiceInstance::write_pipe(core::ProtocolQueue::node_t&& node)
{
	_pipecli->send(std::move(node));
}





