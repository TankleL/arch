#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#endif

#include "dllutils.hpp"

using namespace std;
using namespace arch;
using namespace arch::osys;

#if defined(_WIN32) || defined(WIN32)
dll_handler_t arch::osys::dll_invalid_handler = static_cast<dll_handler_t>(INVALID_HANDLE_VALUE);
#endif

dll_handler_t arch::osys::dll_load(const std::string& filepath)
{
	dll_handler_t handle = dll_invalid_handler;

	handle = ::LoadLibraryA(filepath.c_str());
	if (!handle)
	{
		handle = dll_invalid_handler;
	}

	return handle;
}

dll_raw_symbol_t arch::osys::dll_symbol(dll_handler_t handle, const char* name)
{
	return static_cast<dll_raw_symbol_t>(::GetProcAddress(static_cast<::HMODULE>(handle), name));
}

void arch::osys::dll_unload(dll_handler_t handle)
{
	::FreeLibrary(static_cast<::HMODULE>(handle));
}

