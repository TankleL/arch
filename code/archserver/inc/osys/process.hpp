#include "pre-req.hpp"


namespace osys
{

#if defined(WIN32) || defined(_WIN32)
	typedef struct _process_handle
	{
		_process_handle()
			: hproc(nullptr)
		{}
		
		std::unique_ptr<char>	cmdline;
		void*					hproc;
	} process_handle_t;
#else
	typedef int process_handle_t;
#endif

	process_handle_t create_process(
		const std::string& appname,
		const std::string& cmdline,
		const std::string& workingdir);
	void close_process_handle(process_handle_t handle);

}





