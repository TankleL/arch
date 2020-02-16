#include "pre-req.hpp"
#include "asvc-pipeserver.hpp"

using namespace archsvc;

int main(int argc, char** argv)
{
	if (argc < 2)
		return -1;

	const char* pipename = argv[1];
	PipeServer svc_server(pipename);

    return 0;
}



