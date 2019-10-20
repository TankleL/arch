#include "libam.hpp"

using namespace std;
using namespace arch;
using namespace libam;

IArchModule* libam::_global_amodule_instance = nullptr;

extern "C" __DECLSPEC_DLLEXPORT int		arch_module_init(void)
{
	return _global_amodule_instance->Init();
}

extern "C" __DECLSPEC_DLLEXPORT void	arch_module_uninit(void)
{
	_global_amodule_instance->Uninit();
}

extern "C" __DECLSPEC_DLLEXPORT void	arch_service_processor(arch::ArchMessage& onode, const arch::ArchMessage& inode)
{
	_global_amodule_instance->service_processor(onode, inode);
}
