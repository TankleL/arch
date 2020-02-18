#include "process.hpp"

using namespace osys;

#if defined(_WIN32)
#	include <Windows.h>

process_handle_t osys::create_process(
		const std::string& appname,
		const std::string& cmdline,
		const std::string& workingdir)
{
	STARTUPINFOA startup_info;
	PROCESS_INFORMATION process_info;

	memset(&startup_info, 0, sizeof(startup_info));
	memset(&process_info, 0, sizeof(process_info));

	std::string fullappname = appname + ".exe";

	char commandline[32768] = { 0 };
	memcpy(commandline, cmdline.data(), cmdline.length());

	if (::CreateProcessA(
			fullappname.c_str(),
			commandline,
			NULL,
			NULL,
			false,
			0,
			NULL,
			workingdir.c_str(),
			&startup_info,
			&process_info))
	{
		return process_info.hProcess;
	}
	else
	{
		return nullptr;
	}
}

void osys::close_process_handle(process_handle_t handle)
{
	::CloseHandle(handle);
}



#endif










