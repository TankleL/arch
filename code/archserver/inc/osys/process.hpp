#include "pre-req.hpp"


namespace osys
{

	typedef void* process_handle_t;

	process_handle_t create_process(
		const std::string& appname,
		const std::string& cmdline,
		const std::string& workingdir);
	void close_process_handle(process_handle_t handle);

}





