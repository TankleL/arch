#include <iostream>
#include "mdldemo.hpp"

using namespace std;

DEF_ARCMODULE(ModuleDemo);

int ModuleDemo::Init()
{
	cout << "module mdldemo has been loaded" << endl;
	return 0;
}

void ModuleDemo::Uninit()
{
	cout << "module mdldemo has been unloaded" << endl;
}

void ModuleDemo::service_processor(arch::ArchMessage& onode, const arch::ArchMessage& inode)
{
	cout << "mdldemo: this is an echo protocol module" << endl;
	cout << "mdldemo: processing a request ..." << endl;

	onode.set_conn_ctrl_type(arch::CCT_Close_AfterSend);

	const arch::ProtocolObjectWebSocket* sobj = static_cast<const arch::ProtocolObjectWebSocket*>(inode.get_data_object());
	arch::ProtocolObjectWebSocket* dobj = new arch::ProtocolObjectWebSocket();
	
	arch::msg_frame_t	rsp_frame;

	bool ff = true;
	for (const auto& fr : sobj->_ioframes)
	{
		if (ff)
		{
			if (fr.data.size() > 3)
			{
				rsp_frame.data.insert(rsp_frame.data.end(), fr.data.begin() + 4, fr.data.end());
			}
			ff = false;
		}
		else
		{
			rsp_frame.data.insert(rsp_frame.data.end(), fr.data.begin(), fr.data.end());
		}
	}

	rsp_frame.fin = true;
	rsp_frame.opcode = arch::WSO_Binary;
	rsp_frame.target_len = rsp_frame.data.size();
	dobj->_ioframes.push_back(std::move(rsp_frame));

	onode.set_data_object(dobj);

	cout << "mdldemo: respond a request" << endl;
}
