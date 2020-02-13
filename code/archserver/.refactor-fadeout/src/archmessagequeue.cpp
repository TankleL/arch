#include "archmessagequeue.hpp"

using namespace std;
using namespace arch;

ArchMessageQueue::ArchMessageQueue()
{}

ArchMessageQueue::~ArchMessageQueue() noexcept
{}

void ArchMessageQueue::push(ArchMessage&& node)
{
	{
		std::unique_lock<std::mutex> lock(_mtx);
		_queue.push(std::move(node));
	}

	if (_delegate)
	{
		_delegate->after_push(*this);
	}
}

bool ArchMessageQueue::pop(ArchMessage& node)
{
	bool res = false;

	{
		std::unique_lock<std::mutex> lock(_mtx);
		if (_queue.size() > 0)
		{
			node.acquire(_queue.front());
			_queue.pop();
			res = true;
		}
	}

	if (_delegate && res)
	{
		_delegate->after_pop(*this, res);
	}

	return res;
}

size_t ArchMessageQueue::size() const noexcept
{
	return _queue.size();
}

void ArchMessageQueue::set_delegate(std::shared_ptr<IArchMessageQueueDelegate> the_delegate) noexcept
{
	_delegate = the_delegate;
}


