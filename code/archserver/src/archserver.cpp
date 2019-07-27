#include "archserver-prereq.hpp"
#include "config.hpp"
#include "dllutils.hpp"
#include "archmessage.hpp"
#include "module.hpp"

using namespace std;
using namespace arch;

int main(int argc, char** argv)
{
	config::load_config(argv[1]);

	ModuleManager	module_mgr;
	module_mgr.load_all_modules();
	

	module_mgr.unload_all_modules();

	return 0;
}