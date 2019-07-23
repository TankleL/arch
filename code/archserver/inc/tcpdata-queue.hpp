#pragma once

#include "archserver-prereq.hpp"
#include "tcpdata-queue-node.hpp"

namespace arch
{
	class TCPDataQueue;
	class ITCPDataQueueDelegate
	{
	public:
		virtual void after_push(TCPDataQueue& this_queue) noexcept = 0;
		virtual void after_pop(TCPDataQueue& this_queue) noexcept = 0;
	};

	class TCPDataQueue
	{
	public:
		TCPDataQueue();
		~TCPDataQueue() noexcept;

	public:
		void	push(TCPDataQueueNode* node);
		bool	pop(TCPDataQueueNode** node);
		
		size_t	size() const noexcept;
		void	set_delegate(ITCPDataQueueDelegate* the_delegate) noexcept;

	protected:
		std::queue<TCPDataQueueNode*>* _queue;
		std::mutex* _mtx;
		ITCPDataQueueDelegate* _delegate;
	};
}



