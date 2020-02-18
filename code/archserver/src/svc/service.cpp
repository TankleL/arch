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
{}

svc::Service::Service(Service&& rhs) noexcept
	: _id(rhs._id)
	, _name(std::move(rhs._name))
	, _path(std::move(rhs._path))
	, _workingdir(std::move(rhs._workingdir))
	, _assembly(std::move(rhs._assembly))
	, _insts(std::move(rhs._insts))
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
		_insts.insert({
				id,
				std::make_unique<ServiceInstance>(
					*this,
					id,
					inque,
					outque)
		});
	}
	else
	{
		throw ArchException_ItemAlreadyExist();
	}
}





