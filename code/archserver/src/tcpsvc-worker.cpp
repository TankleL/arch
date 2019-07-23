#include "tcpsvc-worker.hpp"
#include "archexception.hpp"

using namespace arch;

TCPServiceWorker::TCPServiceWorker(ArchMessageQueue* in_queue, ArchMessageQueue* out_queue, IServiceProcessor* svc) noexcept
	: _this_thread(nullptr)
	, _in_queue(in_queue)
	, _out_queue(out_queue)
	, _svc(svc)
	, _abort(false)
{}


TCPServiceWorker::~TCPServiceWorker()
{
	safe_delete(_this_thread);
}

void TCPServiceWorker::start()
{
	if (_this_thread == nullptr)
	{
		_this_thread = new std::thread(std::bind(&TCPServiceWorker::_thread, this));
	}
	else
	{
		throw ArchException_ItemAlreadyExist();
	}
}

void TCPServiceWorker::abort()
{
	_abort = true;
}

void TCPServiceWorker::wait()
{
	_this_thread->join();
	safe_delete(_this_thread);
}

void TCPServiceWorker::_thread()
{
	while (!_abort)
	{
		if (_in_queue->size() > 0)
		{
			ArchMessage*	inode = nullptr;
			if (_in_queue->pop(&inode))
			{
				_svc->process(*_out_queue, *inode);
				delete inode;
			}
			else
			{
				// TODO: change this pulling method into the being notified method.
				std::this_thread::sleep_for(std::chrono::microseconds(10));
			}
		}
		else
		{
			// TODO: change this pulling method into the being notified method.
			std::this_thread::sleep_for(std::chrono::microseconds(10));
		}
	}
}
