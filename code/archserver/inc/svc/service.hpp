#pragma once

#include "pre-req.hpp"
#include "protocol.hpp"
#include "service-inst.hpp"
#include "protocol-queue.hpp"


namespace svc
{

	class Service
	{
		friend ServiceInstance;
		UNCOPYABLE(Service);
	public:
		Service(
			const core::IProtocolData::service_id_t id,
			const std::string& name,
			const std::string& path,
			const std::string& workingdir,
			const std::string& assembly);
		Service(Service&& rhs) noexcept;
		Service& operator=(Service&& rhs) noexcept;
		~Service();

		core::IProtocolData::service_id_t get_id() const;

		void new_instance(
			const core::IProtocolData::service_inst_id_t id,
			const std::shared_ptr<core::ProtocolQueue>& inque,
			const std::shared_ptr<core::ProtocolQueue>& outque);

		void delete_instance(
			const core::IProtocolData::service_inst_id_t& id);

		ServiceInstance* get_instance(
			const core::IProtocolData::service_inst_id_t& id) const;
		ServiceInstance* get_instance_random();

	private:
		core::IProtocolData::service_id_t	_id;
		std::string			_name;
		std::string			_path;
		std::string			_workingdir;
		std::string			_assembly;
		std::unordered_map<
			core::IProtocolData::service_inst_id_t,
			std::unique_ptr<ServiceInstance>>	_insts;
		std::vector<
			core::IProtocolData::service_inst_id_t> _inst_ids;
		int _top_inst_id_idx;
	};

}





