#pragma once
#include "pre-req.hpp"
#include "protocol.hpp"

namespace svc
{

	class ServiceInstance
	{
		ServiceInstance(const ServiceInstance& rhs) = delete;
		ServiceInstance& operator=(const ServiceInstance& rhs) = delete;

	public:
		typedef uint16_t service_inst_id_t;

	public:
		ServiceInstance(const service_inst_id_t& id);
		ServiceInstance(ServiceInstance&& rhs) noexcept;
		ServiceInstance& operator=(ServiceInstance&& rhs) noexcept;
		~ServiceInstance() noexcept;

	private:
		service_inst_id_t	_id;
	};

}


