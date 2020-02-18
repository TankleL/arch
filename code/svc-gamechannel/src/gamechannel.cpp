#include "pre-req.hpp"
#include "asvc-pipeserver.hpp"

using namespace archsvc;

int main(int argc, char** argv)
{

	system("pause");

	system("pause");

	if (argc < 2)
		return -1;

	const char* pipename = argv[1];
	PipeServer svc_server(pipename);



	svc_server.wait();

    return 0;
}



