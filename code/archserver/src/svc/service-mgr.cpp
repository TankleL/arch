#include "service-mgr.hpp"
#include "ex-general.hpp"

using namespace svc;
using namespace core;


std::unordered_map<
	core::IProtocolData::service_id_t,
	std::shared_ptr<Service>>	ServiceMgr::_svcs;

std::unordered_map<
	std::size_t,
	std::unique_ptr<ServiceMgr::ioqueues_t>> ServiceMgr::_svc_ioqueues;


bool svc::ServiceMgr::dispatch_protocol_data(
	core::ProtocolQueue::node_t&& node)
{
	bool result = true;
	if (!node.data.expired())
	{
		auto protodata = node.data.lock();
		auto svc_id = protodata->service_id();
		auto inst_id = protodata->service_inst_id();

		const auto& svc = _svcs.find(svc_id);
		if (svc != _svcs.cend())
		{
			if (inst_id != 0)
			{
				svc->second->get_instance(inst_id)->write_pipe(std::move(node));
			}
			else
			{  // assign an instance randomly for the request
				core::IProtocolData::service_inst_id_t inst_id;
				svc->second->get_instance_random(inst_id)->write_pipe(std::move(node));
				protodata->set_service_inst_id(inst_id);
			}
		}
	}
	else
	{
		result = false;
	}
	return result;
}

svc::ServiceMgr::ioqueues_t&
svc::ServiceMgr::get_ioques(
	const core::IProtocolData::service_id_t& svc_id,
	const core::IProtocolData::service_inst_id_t& inst_id)
{
	const auto& ques = _svc_ioqueues.find(
		_ioqueue_id(svc_id, inst_id));
	if (ques != _svc_ioqueues.cend())
	{
		return *(ques->second.get());
	}
	throw ArchException_ItemNotFound();
}

void svc::ServiceMgr::add_service(
	const std::shared_ptr<Service>& service)
{
	const auto& svc = _svcs.find(service->get_id());
	if (svc == _svcs.cend())
	{
		_svcs.insert({
			service->get_id(),
			service});
	}
	else
	{
		throw ArchException_ItemAlreadyExist();
	}
}

void svc::ServiceMgr::startup_service(
	const core::IProtocolData::service_id_t& svc_id,
	const core::IProtocolData::service_inst_id_t& inst_id)
{
	const auto& service = _svcs.find(svc_id);
	if (service != _svcs.cend())
	{
		auto ioques = 
			std::make_unique<ioqueues_t>();
		auto inque = ioques->inque;
		auto outque = ioques->outque;


		_svc_ioqueues[_ioqueue_id(svc_id, inst_id)] = std::move(ioques);

		service->second->new_instance(
			inst_id,
			inque,
			outque
		);
	}
	else
	{
		throw ArchException_ItemNotFound();
	}
}

std::size_t svc::ServiceMgr::_ioqueue_id(
	core::IProtocolData::service_id_t svc_id,
	core::IProtocolData::service_inst_id_t inst_id)
{
	size_t h1 = std::hash<core::IProtocolData::service_id_t>{}(svc_id);
	size_t h2 = std::hash<core::IProtocolData::service_inst_id_t>{}(inst_id);
	return h1 ^ (h2 << 1);
}

void svc::ServiceMgr::pull_protocol_data(
	std::vector<core::ProtocolQueue::node_t>& results)
{
	for (auto& ioques : _svc_ioqueues)
	{
		if (ioques.second->inque->size())
		{
			ioques.second->inque->pop(results);
		}
	}
}




