#pragma once
#include "pre-req.hpp"
#include "service-inst.hpp"
#include "protocol-queue.hpp"

namespace svc
{

	class ServiceManager
	{
		STATIC_CLASS(ServiceManager);
	public:
		typedef struct _ioqueues
		{
			core::ProtocolQueue	inque;
			core::ProtocolQueue	outque;
		} ioqueues_t;

	public:
		static void new_instance(
			core::IProtocolData::service_id_t svc_id,
			const core::IProtocolData::service_inst_id_t& svc_inst_id);
		static ioqueues_t& get_ioques(const core::IProtocolData::service_inst_id_t& svc_inst_id);
		static void boot_all_instances();
		static void dispatch_protocol_data(core::ProtocolQueue::node_t&& node);

	private:
		static std::unordered_map<
			core::IProtocolData::service_id_t,
			std::vector<std::unique_ptr<ServiceInstance>>> _svc_insts;

		static std::unordered_map<
			core::IProtocolData::service_inst_id_t,
			std::unique_ptr<ioqueues_t>> _svc_ioqueues;
	};

}


