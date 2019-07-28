#include <iostream>
#include "module-interfaces.hpp"
#include "protocols.hpp"

using namespace std;

extern "C" __DECLSPEC_DLLEXPORT int arch_module_init(void)
{
	cout << "module mdldemo has been loaded" << endl;
	return 0;
}

extern "C" __DECLSPEC_DLLEXPORT void arch_module_uninit(void)
{
	cout << "module mdldemo has been unloaded" << endl;
}

extern "C" __DECLSPEC_DLLEXPORT void arch_service_processor(arch::ArchMessage& onode, const arch::ArchMessage& inode)
{
	cout << "mdldemo: processing a request" << endl;

	onode.set_conn_ctrl_type(arch::CCT_Close_AfterSend);

	arch::ProtocolObjectHttp* dobj = new arch::ProtocolObjectHttp();
	dobj->status_code = 200;

	onode.set_data_object(dobj);
}
