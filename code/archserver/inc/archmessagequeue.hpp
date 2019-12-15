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
		virtual void after_pop(ArchMessageQueue& this_queue, bool pop_succeeded) noexcept = 0;
	};

	class ArchMessageQueue
	{
	public:
		ArchMessageQueue();
		virtual ~ArchMessageQueue() noexcept;

	public:
		void	push(ArchMessage&& node);
		bool	pop(ArchMessage& node);
		
		size_t	size() const noexcept;
		void	set_delegate(std::shared_ptr<IArchMessageQueueDelegate> the_delegate) noexcept;

	protected:
		std::queue<ArchMessage>						_queue;
		std::mutex									_mtx;
		std::shared_ptr<IArchMessageQueueDelegate>	_delegate;
	};
}



