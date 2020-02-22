#pragma once

#include "asvc-pipeserver.hpp"
#include <memory>

namespace archsvc
{

	class ServiceInstance
	{
	public:
		typedef uint16_t id_t;

	public:
		ServiceInstance(
			const id_t& id,
			const PipeServer::receiver_t& receiver);
		~ServiceInstance();
		
	public:
		void run();

	private:
		std::unique_ptr<PipeServer>	_pipesvr;
		id_t						_id;
		PipeServer::receiver_t		_receiver;
	};

}





