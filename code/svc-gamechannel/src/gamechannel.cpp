#include "pre-req.hpp"
#include "asvc-pipeserver.hpp"

using namespace archsvc;

int main(int argc, char** argv)
{
	if (argc < 1)
		return -1;

	const char* pipename = argv[0];
	PipeServer svc_server(pipename);



	svc_server.wait();

    return 0;
}



