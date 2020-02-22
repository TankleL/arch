#include "service.hpp"
#include "ex-general.hpp"

using namespace svc;

svc::Service::Service(
			const core::IProtocolData::service_id_t id,
			const std::string& name,
			const std::string& path,
			const std::string& workingdir,
			const std::string& assembly)
	: _id(id)
	, _name(name)
	, _path(path)
	, _workingdir(workingdir)
	, _assembly(assembly)
	, _top_inst_id_idx(0)
{}

svc::Service::Service(Service&& rhs) noexcept
	: _id(rhs._id)
	, _name(std::move(rhs._name))
	, _path(std::move(rhs._path))
	, _workingdir(std::move(rhs._workingdir))
	, _assembly(std::move(rhs._assembly))
	, _insts(std::move(rhs._insts))
	, _inst_ids(std::move(rhs._inst_ids))
	, _top_inst_id_idx(rhs._top_inst_id_idx)
{}

svc::Service& 
svc::Service::operator=(Service&& rhs) noexcept
{
	_id = rhs._id;
	_name = std::move(rhs._name);
	_path = std::move(rhs._path);
	_workingdir = std::move(rhs._workingdir);
	_assembly = std::move(rhs._assembly);
	_insts = std::move(rhs._insts);
	_inst_ids = std::move(rhs._inst_ids);
	_top_inst_id_idx = rhs._top_inst_id_idx;

	return *this;
}

svc::Service::~Service()
{}

core::IProtocolData::service_id_t svc::Service::get_id() const
{
	return _id;
}

void svc::Service::new_instance(
	const core::IProtocolData::service_inst_id_t id,
	const std::shared_ptr<core::ProtocolQueue>& inque,
	const std::shared_ptr<core::ProtocolQueue>& outque)
{
	const auto& inst = _insts.find(id);
	if (inst == _insts.cend())
	{
		auto new_inst = std::make_unique<ServiceInstance>(
			*this,
			id,
			inque,
			outque);

		_insts.insert({id, std::move(new_inst)});
		_inst_ids.push_back(id);
	}
	else
	{
		throw ArchException_ItemAlreadyExist();
	}
}

ServiceInstance* svc::Service::get_instance(
	const core::IProtocolData::service_inst_id_t& id) const
{
	// no need to check whether the instance exists or not.
	// if not, just let server go down because there must be
	// some big problems with configuration.
	const auto& inst = _insts.find(id);
	return inst->second.get();
}

ServiceInstance* svc::Service::get_instance_random()
{
	return _insts[
		_inst_ids[
			(core::IProtocolData::service_inst_id_t)
				(_top_inst_id_idx++ % _inst_ids.size())]].get();
}





