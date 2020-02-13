#pragma once

#include "archserver-prereq.hpp"
#include "dllutils.hpp"
#include "protocols.hpp"
#include "archmessage.hpp"
#include "service-processor.hpp"

namespace arch
{
	class IModule
		: public IServiceProcessor
		, public IDisposable
	{
	public:
		virtual bool	init() = 0;
		virtual void	uninit() = 0;
	};

}

