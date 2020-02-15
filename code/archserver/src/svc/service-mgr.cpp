#include "service-mgr.hpp"

using namespace svc;
using namespace core;


std::unordered_map<
	core::IProtocolData::service_id_t,
	std::vector<std::unique_ptr<ServiceInstance>>> ServiceManager::_svc_insts;

std::unordered_map<
	core::IProtocolData::service_id_t,
	std::vector<std::unique_ptr<ServiceManager::ioqueues_t>>> ServiceManager::_svc_ioqueues;

void svc::ServiceManager::new_instance(
	core::IProtocolData::service_id_t svc_id,
	const ServiceInstance::service_inst_id_t& svc_inst_id)
{
	_svc_insts[svc_id].push_back(
		std::make_unique<ServiceInstance>(svc_inst_id));
}

void svc::ServiceManager::boot_all_instances()
{}

void svc::ServiceManager::dispatch_protocol_data(
	core::ProtocolQueue::node_t&& node)
{}



