#include "tcpsvc-workermgr.hpp"

using namespace std;
using namespace arch;

TCPServiceWorkerManager::TCPServiceWorkerManager(
	ArchMessageQueue* in_queue,
	ArchMessageQueue* out_queue,
	ModuleManager* mm,
	int core_number)
	: _core_nums(core_number)
{
	for (int i = 0; i < _core_nums; ++i)
	{
		_workers.push_back(new TCPServiceWorker(in_queue, out_queue, mm));
	}
}

TCPServiceWorkerManager::~TCPServiceWorkerManager()
{
	for (auto& worker : _workers)
	{
		safe_delete(worker);
	}
}

void TCPServiceWorkerManager::start()
{
	for (auto& worker : _workers)
	{
		worker->start();
	}
}

void TCPServiceWorkerManager::abort()
{
	for (auto& worker : _workers)
	{
		worker->abort();
	}
}

void TCPServiceWorkerManager::wait()
{
	for (auto& worker : _workers)
	{
		worker->wait();
	}
}

