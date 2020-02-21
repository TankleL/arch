#include "process.hpp"

using namespace osys;

#if defined(_WIN32)
#	include <Windows.h>

process_handle_t osys::create_process(
		const std::string& appname,
		const std::string& cmdline,
		const std::string& workingdir)
{
	process_handle_t res;

	STARTUPINFOA startup_info;
	PROCESS_INFORMATION process_info;

	memset(&startup_info, 0, sizeof(startup_info));
	memset(&process_info, 0, sizeof(process_info));

	std::string fullappname = appname + ".exe";

	res.cmdline =
		std::make_unique<char>((int)cmdline.length() + 1);
	memset(res.cmdline.get(), 0, cmdline.length() + 1);
	memcpy(res.cmdline.get(), cmdline.data(), cmdline.length());

	if (::CreateProcessA(
			fullappname.c_str(),
			res.cmdline.get(),
			NULL,
			NULL,
			false,
			0,
			NULL,
			workingdir.c_str(),
			&startup_info,
			&process_info))
	{
		::CloseHandle(process_info.hThread);
		res.hproc = (void*)process_info.hProcess;
	}

	return res;
}

void osys::close_process_handle(process_handle_t handle)
{
	::CloseHandle((HANDLE)handle.hproc);
}



#endif










