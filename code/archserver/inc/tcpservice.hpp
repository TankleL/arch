#pragma once

#include "archserver-prereq.hpp"
#include "disposable.hpp"
#include "acquirable.hpp"
#include "tcpdata-queue.hpp"

namespace arch
{

	class IServiceDataObject
		: public IDisposable
		, public IAcquriable<IServiceDataObject>
	{};

	class IServiceProcessor
	{
	public:
		virtual void process(TCPDataQueue& out_queue, const TCPDataQueueNode& inode) = 0;
	};

}

