#pragma once
#include "pre-req.hpp"
#include "protocol.hpp"
#include "pipeclient.hpp"
#include "process.hpp"

namespace svc
{

	class Service;
	class ServiceInstance
	{
		ServiceInstance(const ServiceInstance& rhs) = delete;
		ServiceInstance& operator=(const ServiceInstance& rhs) = delete;

	public:
		ServiceInstance(
			const Service& service,
			const core::IProtocolData::service_inst_id_t& id,
			const std::shared_ptr<core::ProtocolQueue>& inque,
			const std::shared_ptr<core::ProtocolQueue>& outque);
		ServiceInstance(ServiceInstance&& rhs) noexcept;
		ServiceInstance& operator=(ServiceInstance&& rhs) noexcept;
		~ServiceInstance() noexcept;

	public:
		void write_pipe(core::ProtocolQueue::node_t&& node);

	private:
		void _pipe_outque_guard(core::ProtocolQueue::node_t& node);
		std::string _pipename() const;

	private:
		std::unique_ptr<core::PipeClient>		_pipecli;
		core::IProtocolData::service_id_t		_svc_id;
		core::IProtocolData::service_inst_id_t	_id;
	};

}


