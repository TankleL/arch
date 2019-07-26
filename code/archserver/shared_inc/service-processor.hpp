#pragma once

#include "disposable.hpp"
#include "acquirable.hpp"
#include "archmessagequeue.hpp"

namespace arch
{

	class IServiceProcessor
	{
	public:
		virtual void process(ArchMessageQueue& out_queue, const ArchMessage& inode) = 0;
	};

}

