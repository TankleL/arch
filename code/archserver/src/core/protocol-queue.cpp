#include "protocol-queue.hpp"

core::ProtocolQueue::ProtocolQueue() noexcept
{}

core::ProtocolQueue::ProtocolQueue(ProtocolQueue&& rhs) noexcept
	: _queue(std::move(rhs._queue))
{}

core::ProtocolQueue& core::ProtocolQueue::operator=(ProtocolQueue&& rhs) noexcept
{
	_queue = std::move(rhs._queue);
	return *this;
}

core::ProtocolQueue::~ProtocolQueue()
{}


void core::ProtocolQueue::push(node_t&& node)
{
	std::unique_lock<std::mutex>	lock(_mtx_push_pop);
	_queue.push_back(std::move(node));
}

bool core::ProtocolQueue::pop(node_t& node)
{
	bool result = false;

	std::unique_lock<std::mutex>	lock(_mtx_push_pop);
	if (_queue.size())
	{
		node = std::move(_queue.front());
		_queue.pop_front();
		result = true;
	}

	return result;
}

size_t core::ProtocolQueue::size() const noexcept
{
	return _queue.size();
}




