#pragma once

#include "archserver-prereq.hpp"
#include "archmessagequeue.hpp"
#include "service-processor.hpp"

namespace arch
{

	class TCPServiceWorker
	{
	public:
		TCPServiceWorker(ArchMessageQueue* in_queue, ArchMessageQueue* out_queue, IServiceProcessor* svc) noexcept;
		~TCPServiceWorker();

	public:
		void start();
		void abort();
		void wait();

	protected:
		void _thread();

	protected:
		std::thread*		_this_thread;
		ArchMessageQueue*		_in_queue;
		ArchMessageQueue*		_out_queue;
		IServiceProcessor*	_svc;
		bool				_abort;
	};

}

