#include "pre-req.hpp"
#include "asvc-service-instance.hpp"
#include "asvc-pipeserver.hpp"
#include "asvc-ccf.hpp"
#include "getopt.h"

using namespace archsvc;
using namespace archproto;

bool on_receive(
	uint16_t conn_id,
	uint16_t ccf,
	ProtocolType proto,
	std::unique_ptr<archproto::IProtocolData>&& data,
	PipeServer& pipe)
{
	pipe.write(
		conn_id,
		ccf,
		proto,
		std::move(data));
	return true;
}

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


	ServiceInstance inst(
		svc_id,
		svc_inst_id,
		std::bind(
			on_receive,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4,
			std::placeholders::_5));
	inst.run();

    return 0;
}



