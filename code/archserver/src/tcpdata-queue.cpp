#include "tcpdata-queue.hpp"

using namespace std;
using namespace arch;

TCPDataQueue::TCPDataQueue()
	: _queue(nullptr)
	, _mtx(nullptr)
	, _delegate(nullptr)
{
	try
	{
		_queue = new std::queue<TCPDataQueueNode*>();
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

TCPDataQueue::~TCPDataQueue() noexcept
{
	delete _queue;
	delete _mtx;
}

void TCPDataQueue::push(TCPDataQueueNode* node)
{
	std::unique_lock<std::mutex>	lock(*_mtx);
	_queue->push(node);
	lock.unlock();

	if (_delegate)
	{
		_delegate->after_push(*this);
	}
}

bool TCPDataQueue::pop(TCPDataQueueNode** node)
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

size_t TCPDataQueue::size() const noexcept
{
	return _queue->size();
}

void TCPDataQueue::set_delegate(ITCPDataQueueDelegate* the_delegate) noexcept
{
	_delegate = the_delegate;
}


