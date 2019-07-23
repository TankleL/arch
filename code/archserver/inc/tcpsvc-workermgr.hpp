#pragma once

#include "archserver-prereq.hpp"
#include "tcpsvc-worker.hpp"

namespace arch
{

	class TCPServiceWorkerManager
	{
	public:
		TCPServiceWorkerManager(
			TCPDataQueue* in_queue,
			TCPDataQueue* out_queue,
			IServiceProcessor* svc,
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
