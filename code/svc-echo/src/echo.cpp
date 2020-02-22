#include "pre-req.hpp"
#include "asvc-service-instance.hpp"
#include "asvc-pipeserver.hpp"
#include "getopt.h"

using namespace archsvc;

bool on_receive(
	std::vector<uint8_t>&& data,
	PipeServer& pipe)
{
	return true;
}

int main(int argc, char** argv)
{
	ServiceInstance::id_t svc_inst_id;

	int opt;
	std::stringstream ioss;
	while ((opt = getopt(argc, argv, "i:")) != -1)
	{
		switch (opt)
		{
		case 'i': // service instance id
			ioss << optarg;
			ioss >> svc_inst_id;
			break;
			
		default:
			std::cerr
				<< "fatal error: bad arguments."
				<< std::endl;
			exit(-1);
			break;
		}
	}

	ServiceInstance inst(
		svc_inst_id,
		on_receive);
	inst.run();

    return 0;
}



