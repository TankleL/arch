#pragma once

#include "archserver-prereq.hpp"
#include "archmessagequeue.hpp"
#include "service-processor.hpp"
#include "modulemgr.hpp"

namespace arch
{

	class TCPServiceWorker
	{
	public:
		TCPServiceWorker(ArchMessageQueue* in_queue, ArchMessageQueue* out_queue, ModuleManager* mm) noexcept;
		~TCPServiceWorker();

	public:
		void start();
		void abort();
		void wait();

	protected:
		void _thread();

	protected:
		std::thread*			_this_thread;
		ArchMessageQueue*		_in_queue;
		ArchMessageQueue*		_out_queue;
		ModuleManager*			_mm;
		bool					_abort;
	};

}

