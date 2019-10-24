#include <iostream>
#include "libam.hpp"
#include "httpd.hpp"

using namespace std;

DEF_ARCMODULE(ModuleHTTPD);

int ModuleHTTPD::Init()
{
	cout << "module mdlhttpd has been loaded" << endl;
	return 0;
}

void ModuleHTTPD::Uninit()
{}

void ModuleHTTPD::service_processor(arch::ArchMessage& onode, const arch::ArchMessage& inode)
{}


