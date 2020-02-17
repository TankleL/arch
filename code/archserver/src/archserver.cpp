#include "pre-req.hpp"
#include "tcpserver.hpp"
#include "service-mgr.hpp"
#include "config-mgr.hpp"
#include "config-master.hpp"

using namespace core;
using namespace svc;
using namespace config;

int main(int argc, char** argv)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::string svr_root = std::filesystem::absolute(TEST_GSVR_ENV_PATH).u8string();
#else
	std::string svr_root = argv[1];
#endif

	ConfigMgr::load_configs(svr_root + "/config");

	ServiceManager::new_instance(101, 1);
	ServiceManager::boot_all_instances();

	if (MasterConfig::listen.sockproto == MasterConfig::SP_TCP)
	{
		TCPServer	svr(
			MasterConfig::listen.ipaddr,
			MasterConfig::listen.port,
			MasterConfig::listen.backlog);
		svr.wait();
	}

	return 0;
}


