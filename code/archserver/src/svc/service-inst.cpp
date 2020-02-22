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
	, _svc_id(service.get_id())
{
	std::ostringstream appname;
	appname << service._path << "/" << service._assembly;
	std::ostringstream oss_cmdline;
	oss_cmdline << "-i" << _pipename();

	if (osys::create_process(
		appname.str(),
		oss_cmdline.str(),
		service._workingdir))
	{
		std::ostringstream oss_pipename;
		oss_pipename << "asvc_data_pipe_" << _pipename();
		_pipecli = std::make_unique<PipeClient>(
			oss_pipename.str(),
			inque,
			outque,
			std::bind(
				&ServiceInstance::_pipe_outque_guard,
				this,
				std::placeholders::_1));
	}
	else
	{
		throw std::runtime_error("failed to create process.");
	}
}

ServiceInstance::ServiceInstance(ServiceInstance&& rhs) noexcept
	: _pipecli(std::move(rhs._pipecli))
	, _id(rhs._id)
	, _svc_id(rhs._svc_id)
{}

ServiceInstance& ServiceInstance::operator=(ServiceInstance&& rhs) noexcept
{
	_pipecli = std::move(rhs._pipecli);
	_id = rhs._id;
	return *this;
}

ServiceInstance::~ServiceInstance()
{}

void ServiceInstance::write_pipe(core::ProtocolQueue::node_t&& node)
{
	_pipecli->send(std::move(node));
}

void ServiceInstance::_pipe_outque_guard(core::ProtocolQueue::node_t& node)
{
	PlainProtocolData& pdata = static_cast<PlainProtocolData&>(*node.sdata);
	pdata.svc_id = _svc_id;
	pdata.svc_inst_id = _id;
}

std::string ServiceInstance::_pipename() const
{
	std::string r1, r2;

	std::stringstream ss1;
	ss1 << _svc_id;
	ss1 >> r1;

	std::stringstream ss2;
	ss2 << _id;
	ss2 >> r2;

	return r1 + r2;
}




