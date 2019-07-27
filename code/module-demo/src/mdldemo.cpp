#include "module-interfaces.hpp"
#include <iostream>

using namespace std;

extern "C" __DECLSPEC_DLLEXPORT int arch_module_init(void)
{
	cout << "module mdldemo has been loaded" << endl;
	return 0;
}

extern "C" __DECLSPEC_DLLEXPORT void arch_module_uninit(void)
{
	cout << "module mdldemo has been unloaded" << endl;
}