#include "module-interfaces.hpp"

extern "C" __DECLSPEC_DLLEXPORT int arch_module_init(void)
{
	return 0;
}

extern "C" __DECLSPEC_DLLEXPORT void arch_module_uninit(void)
{
}