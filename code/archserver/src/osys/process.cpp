#include "process.hpp"

using namespace osys;

#if defined(_WIN32)
#	include <Windows.h>

bool osys::create_process(
		const std::string& appname,
		const std::string& cmdline,
		const std::string& workingdir)
{
	bool res = true;

	STARTUPINFOA startup_info;
	PROCESS_INFORMATION process_info;

	memset(&startup_info, 0, sizeof(startup_info));
	memset(&process_info, 0, sizeof(process_info));

	std::string fullappname = appname + ".exe";

	auto cmd = (char*)malloc(cmdline.length() + 1);
	memset(cmd, 0, cmdline.length() + 1);
	memcpy(cmd, cmdline.data(), cmdline.length());

	if (::CreateProcessA(
			fullappname.c_str(),
			cmd,
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
		::CloseHandle(process_info.hProcess);
	}
	else
	{
		res = false;
	}

	return res;
}



#endif










