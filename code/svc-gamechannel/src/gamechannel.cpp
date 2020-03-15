#include "pre-req.hpp"
#include "getopt.h"
#include "service-inst.hpp"
#include "message.hpp"

using namespace archsvc;
using namespace archproto;


int main(int argc, char** argv)
{
	ServiceInstance::svc_id_t	svc_id = 0;
	ServiceInstance::id_t		svc_inst_id = 0;

	int opt;
	while ((opt = getopt(argc, argv, "s:i:")) != -1)
	{
		std::stringstream ioss;
		switch (opt)
		{
		case 's':
			ioss << optarg;
			ioss >> svc_id;
			break;
			
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

	GameChannelServiceInstance inst(
		svc_id,
		svc_inst_id);
	inst.run();

    return 0;
}



