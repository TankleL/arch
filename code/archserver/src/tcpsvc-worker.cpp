#include "tcpsvc-worker.hpp"
#include "archexception.hpp"

using namespace arch;

TCPServiceWorker::TCPServiceWorker(
	ArchMessageQueue* in_queue, 
	ArchMessageQueue* out_queue,
	ModuleManager* mm) noexcept
	: _this_thread(nullptr)
	, _in_queue(in_queue)
	, _out_queue(out_queue)
	, _mm(mm)
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
			ArchMessage	inode;
			if (_in_queue->pop(inode))
			{
				ArchMessage onode(nullptr, inode.get_hlink(), inode.get_uid());

				IModule* mdl = _mm->get_module(inode.get_data_object()->get_protocol_type());
				if(mdl)	mdl->process(onode, inode);

				_out_queue->push(std::move(onode));
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
