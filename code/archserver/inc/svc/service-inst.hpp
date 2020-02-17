#pragma once
#include "pre-req.hpp"
#include "protocol.hpp"
#include "pipeclient.hpp"

namespace svc
{

	class ServiceInstance
	{
		ServiceInstance(const ServiceInstance& rhs) = delete;
		ServiceInstance& operator=(const ServiceInstance& rhs) = delete;

	public:
		ServiceInstance(const core::IProtocolData::service_inst_id_t& id);
		ServiceInstance(ServiceInstance&& rhs) noexcept;
		ServiceInstance& operator=(ServiceInstance&& rhs) noexcept;
		~ServiceInstance() noexcept;

	private:
		std::unique_ptr<core::PipeClient>		_pipecli;
		core::IProtocolData::service_inst_id_t	_id;
	};

}


