#pragma once

#include <mutex>
#include <queue>
#include "archmessage.hpp"

namespace arch
{
	class ArchMessageQueue;
	class IArchMessageQueueDelegate
	{
	public:
		virtual void after_push(ArchMessageQueue& this_queue) noexcept = 0;
		virtual void after_pop(ArchMessageQueue& this_queue) noexcept = 0;
	};

	class ArchMessageQueue
	{
	public:
		ArchMessageQueue();
		~ArchMessageQueue() noexcept;

	public:
		void	push(ArchMessage* node);
		bool	pop(ArchMessage** node);
		
		size_t	size() const noexcept;
		void	set_delegate(IArchMessageQueueDelegate* the_delegate) noexcept;

	protected:
		std::queue<ArchMessage*>* _queue;
		std::mutex* _mtx;
		IArchMessageQueueDelegate* _delegate;
	};
}



