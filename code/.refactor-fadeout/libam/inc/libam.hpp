#pragma once

#include "acquirable.hpp"
#include "disposable.hpp"
#include "protocols.hpp"
#include "archmessage.hpp"
#include "module-interfaces.hpp"

namespace libam
{

	class IArchModule
	{
	public:
		virtual int		Init() = 0;
		virtual void	Uninit() = 0;
		virtual	void	service_processor(arch::ArchMessage& onode, const arch::ArchMessage& inode) = 0;
	};
	
	extern IArchModule* _global_amodule_instance;

}
