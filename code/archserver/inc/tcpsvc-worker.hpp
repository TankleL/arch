#pragma once

#include "archserver-prereq.hpp"
#include "tcpdata-queue.hpp"
#include "tcpservice.hpp"

namespace arch
{

	class TCPServiceWorker
	{
	public:
		TCPServiceWorker(TCPDataQueue* in_queue, TCPDataQueue* out_queue, IServiceProcessor* svc) noexcept;
		~TCPServiceWorker();

	public:
		void start();
		void abort();
		void wait();

	protected:
		void _thread();

	protected:
		std::thread*		_this_thread;
		TCPDataQueue*		_in_queue;
		TCPDataQueue*		_out_queue;
		IServiceProcessor*	_svc;
		bool				_abort;
	};

}

