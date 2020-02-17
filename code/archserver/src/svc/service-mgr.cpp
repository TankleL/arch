#include "service-mgr.hpp"
#include "ex-general.hpp"

using namespace svc;
using namespace core;


std::unordered_map<
	core::IProtocolData::service_id_t,
	std::vector<std::unique_ptr<ServiceInstance>>> ServiceManager::_svc_insts;

std::unordered_map<
	core::IProtocolData::service_inst_id_t,
	std::unique_ptr<ServiceManager::ioqueues_t>> ServiceManager::_svc_ioqueues;

void svc::ServiceManager::new_instance(
	core::IProtocolData::service_id_t svc_id,
	const core::IProtocolData::service_inst_id_t& svc_inst_id)
{
	_svc_ioqueues[svc_inst_id] = std::make_unique<ioqueues_t>();
	_svc_insts[svc_id].push_back(
		std::make_unique<ServiceInstance>(svc_inst_id));
}

void svc::ServiceManager::boot_all_instances()
{}

void svc::ServiceManager::dispatch_protocol_data(
	core::ProtocolQueue::node_t&& node)
{}

svc::ServiceManager::ioqueues_t&
svc::ServiceManager::get_ioques(const core::IProtocolData::service_inst_id_t& svc_inst_id)
{
	const auto& ques = _svc_ioqueues.find(svc_inst_id);
	if (ques != _svc_ioqueues.cend())
	{
		return *(ques->second.get());
	}
	throw ArchException_ItemNotFound();
}





