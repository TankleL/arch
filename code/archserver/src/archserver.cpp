#include "archserver-prereq.hpp"
#include "config.hpp"
#include "dllutils.hpp"
#include "archmessage.hpp"
#include "module.hpp"
#include "tcpserver.hpp"

using namespace std;
using namespace arch;

int main(int argc, char** argv)
{
#if defined(_DEBUG) || defined(DEBUG)
	config::load_config(TEST_PATH_CONFIG_FILE);
#else
	config::load_config(argv[1]);
#endif

	ModuleManager	module_mgr;
	module_mgr.load_all_modules();
	
	ArchMessageQueue	ique, oque;
	TCPServer	server(&ique, &oque);
	
	server.run(
		&module_mgr,
		config::server_phase.ipaddr,
		config::server_phase.port,
		config::server_phase.backlog,
		config::server_phase.core_num);

	module_mgr.unload_all_modules();

	return 0;
}