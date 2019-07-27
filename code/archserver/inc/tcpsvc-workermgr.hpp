#pragma once

#include "archserver-prereq.hpp"
#include "tcpsvc-worker.hpp"
#include "module.hpp"

namespace arch
{

	class TCPServiceWorkerManager
	{
	public:
		TCPServiceWorkerManager(
			ArchMessageQueue* in_queue,
			ArchMessageQueue* out_queue,
			ModuleManager* mm,
			int core_number = 6);
		~TCPServiceWorkerManager();

	public:
		void start();
		void abort();
		void wait();

	private:
		int								_core_nums;
		std::vector<TCPServiceWorker*>	_workers;
	};

}
