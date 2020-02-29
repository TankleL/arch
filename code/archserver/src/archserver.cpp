#include "pre-req.hpp"
#include "tcpserver.hpp"
#include "service-mgr.hpp"
#include "config-mgr.hpp"
#include "config-master.hpp"
#include "vuint.hpp"

using namespace core;
using namespace svc;
using namespace config;

void signal_int(int sig)
{
	int c = 0;
}

int main(int argc, char** argv)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::string svr_root = std::filesystem::absolute(TEST_GSVR_ENV_PATH).u8string();
	MasterConfig::server_root = svr_root;
#else
	std::string svr_root = argv[1];
	MasterConfig::server_root = svr_root;
#endif

	ConfigMgr::load_configs(svr_root + "/config");

	for (const auto& svc : MasterConfig::services)
	{
		auto service = std::make_shared<Service>(
			svc.svc_id,
			svc.name,
			svc.path,
			svc.workingdir,
			svc.assembly);

		ServiceMgr::add_service(service);

		for (const auto& inst : svc.insts)
		{
			ServiceMgr::startup_service(svc.svc_id, inst.inst_id);
		}
	}


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


