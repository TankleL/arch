#include "pre-req.hpp"
#include "tcpserver.hpp"
#include "service-mgr.hpp"

using namespace core;
using namespace svc;

int main(int argc, char** argv)
{
	ServiceManager::new_instance(101, 1);
	ServiceManager::boot_all_instances();

	TCPServer	svr("0.0.0.0", 8088, 100);
	svr.wait();

	return 0;
}


