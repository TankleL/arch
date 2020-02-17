#pragma once

#include "asvc-pipeserver.hpp"

namespace archsvc
{

	class ServiceInstance
	{
	public:
		typedef uint16_t id_t;

	public:
		ServiceInstance(const id_t& id);

	private:
		PipeServer			_pipesvr;
		id_t				_id;
	};

}





