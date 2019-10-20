#include "libam.hpp"
#include "httpd.hpp"

DEF_ARCMODULE(ModuleHTTPD);

int ModuleHTTPD::Init()
{
	return 0;
}

void ModuleHTTPD::Uninit()
{}

void ModuleHTTPD::service_processor(arch::ArchMessage& onode, const arch::ArchMessage& inode)
{}


