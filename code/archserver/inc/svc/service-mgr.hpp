#pragma once
#include "pre-req.hpp"
#include "protocol-queue.hpp"
#include "service.hpp"

namespace svc
{

	class ServiceMgr
	{
		STATIC_CLASS(ServiceMgr);
	public:
		typedef struct _ioqueues
		{
			_ioqueues()
				: inque(std::make_shared<core::ProtocolQueue>())
				, outque(std::make_shared<core::ProtocolQueue>())
			{}

			std::shared_ptr<core::ProtocolQueue>	inque;
			std::shared_ptr<core::ProtocolQueue>	outque;
		} ioqueues_t;

	public:
		static void add_service(
			const std::shared_ptr<Service>& service);

		static void startup_service(
			const core::IProtocolData::service_id_t& svc_id,
			const core::IProtocolData::service_inst_id_t& inst_id);

		static ioqueues_t& get_ioques(
			const core::IProtocolData::service_id_t& svc_id,
			const core::IProtocolData::service_inst_id_t& svc_inst_id);
		static void dispatch_protocol_data(core::ProtocolQueue::node_t&& node);

	private:
		static std::size_t _ioqueue_id(
			core::IProtocolData::service_id_t svc_id,
			core::IProtocolData::service_inst_id_t inst_id);

	private:
		static std::unordered_map<
			core::IProtocolData::service_id_t,
			std::shared_ptr<Service>>		_svcs;
		static std::unordered_map<
			std::size_t,
			std::unique_ptr<ioqueues_t>>	_svc_ioqueues;
	};

}


