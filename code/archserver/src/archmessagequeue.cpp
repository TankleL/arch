#include "archmessagequeue.hpp"

using namespace std;
using namespace arch;

ArchMessageQueue::ArchMessageQueue()
	: _queue(nullptr)
	, _mtx(nullptr)
	, _delegate(nullptr)
{
	try
	{
		_queue = new std::queue<ArchMessage*>();
	}
	catch (const std::exception& e)
	{
		_queue = nullptr;
		throw e;
	}

	try
	{
		_mtx = new std::mutex();
	}
	catch (const std::exception & e)
	{
		delete _queue;
		_queue = nullptr;
		_mtx = nullptr;
		throw e;
	}
}

ArchMessageQueue::~ArchMessageQueue() noexcept
{
	delete _queue;
	delete _mtx;
}

void ArchMessageQueue::push(ArchMessage* node)
{
	std::unique_lock<std::mutex>	lock(*_mtx);
	_queue->push(node);
	lock.unlock();

	if (_delegate)
	{
		_delegate->after_push(*this);
	}
}

bool ArchMessageQueue::pop(ArchMessage** node)
{
	std::unique_lock<std::mutex>	lock(*_mtx);
	bool res = false;

	if (_queue->size() > 0)
	{
		*node = _queue->front();
		_queue->pop();
		res = true;
	}

	lock.unlock();

	if (_delegate && res)
	{
		_delegate->after_pop(*this);
	}

	return res;
}

size_t ArchMessageQueue::size() const noexcept
{
	return _queue->size();
}

void ArchMessageQueue::set_delegate(IArchMessageQueueDelegate* the_delegate) noexcept
{
	_delegate = the_delegate;
}


