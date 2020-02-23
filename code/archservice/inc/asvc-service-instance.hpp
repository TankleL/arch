#pragma once

#include "asvc-pipeserver.hpp"
#include <memory>

namespace archsvc
{

	class ServiceInstance
	{
	public:
		typedef uint32_t svc_id_t;
		typedef uint16_t id_t;

	public:
		ServiceInstance(
			const svc_id_t& svc_id,
			const id_t& id,
			const PipeServer::receiver_t& receiver);
		~ServiceInstance();
		
	public:
		void run();

	private:
		std::string _pipename() const;

	private:
		std::unique_ptr<PipeServer>	_pipesvr;
		svc_id_t					_svc_id;
		id_t						_id;
		PipeServer::receiver_t		_receiver;
	};

}





