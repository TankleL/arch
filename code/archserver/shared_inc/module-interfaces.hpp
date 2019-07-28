#pragma once
#include "archmessage.hpp"

#if defined(WIN32) || defined(_WIN32)
#	define __DECLSPEC_DLLEXPORT	__declspec(dllexport)
#	define __DECLSPEC_DLLIMPORT	__declspec(dllimport)
#else
#	define __DECLSPEC_DLLEXPORT
#	define __DECLSPEC_DLLIMPORT
#endif

extern "C" __DECLSPEC_DLLEXPORT int		arch_module_init(void);
extern "C" __DECLSPEC_DLLEXPORT void	arch_module_uninit(void);
extern "C" __DECLSPEC_DLLEXPORT void	arch_service_processor(arch::ArchMessage& onode, const arch::ArchMessage& inode);
