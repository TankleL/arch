#include "pre-req.hpp"
#include "tcpserver.hpp"

using namespace core;

int main(int argc, char** argv)
{
	TCPServer	svr("0.0.0.0", 8088, 100);


	svr.wait();

	return 0;
}


