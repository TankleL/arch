#pragma once

#include "archserver-prereq.hpp"
#include "disposable.hpp"
#include "acquirable.hpp"
#include "archmessagequeue.hpp"

namespace arch
{

	class IServiceDataObject
		: public IDisposable
		, public IAcquriable<IServiceDataObject>
	{};

	class IServiceProcessor
	{
	public:
		virtual void process(ArchMessageQueue& out_queue, const ArchMessage& inode) = 0;
	};

}

