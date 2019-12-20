#include <iostream>
#include "mdlarchd.hpp"
#include "message.hpp"

using namespace std;

DEF_ARCMODULE(ModuleArchd);


enum ArchProtocolVersion : int
{
	APV_Unknown,
	APV_0_1		// arch protocol 0.1
};

bool decode_msg(Message& dst, const arch::ProtocolObjectArch& src)
{
	// obj
	if (src.version == APV_0_1 && src.data.size() > 0)
	{
		// decode msg evid
		int evid = 0;
		int evid_len = 0;
		uint8_t d0 = src.data.at(0);
		if ((d0 & 0x80) == 0x0)	// 0 ~ 7 bits
		{
			evid = d0 & 0x7f;
			evid_len = 1;
		}
		else if ((d0 & 0xc0) == 0x80) // 8 ~ 14 bits
		{
			if (src.data.size() < 2)
			{ // bad data
				return false;
			}

			uint8_t d1 = src.data.at(1);
			evid = (d0 & 0x3f) |
				(d1 << 6);
			evid_len = 2;
		}
		else if ((d0 & 0xe0) == 0xc0) // 15 ~ 21 bits
		{
			if (src.data.size() < 3)
			{ // bad data
				return false;
			}

			uint8_t d1 = src.data.at(1);
			uint8_t d2 = src.data.at(2);
			evid = (d0 & 0x1f) |
				(d1 << 5) |
				(d2 << 13);
			evid_len = 3;
		}
		else if ((d0 & 0xf0) == 0xe0) // 22 ~ 24 bits
		{
			if (src.data.size() < 4)
			{ // bad data
				return false;
			}

			evid = (src.data.at(1)) |
				(src.data.at(2) << 8) |
				(src.data.at(3) << 16);
			evid_len = 4;
		}
		else
		{
			return false;
		}

		if (src.data.size() - evid_len == 0)
		{
			// we don't want to support empty message.
			return false;
		}

		Message msg((uint16_t)src.data.size() - evid_len);
		msg.set_id(MessageUtil::make_id(0, evid));
		msg.set_data(src.data, (uint16_t)evid_len);
		dst.acquire(msg);
		return true;
	}
	else
	{ // return false to close the connection.
		return false;
	}
}

int ModuleArchd::Init()
{
	cout << "module mdlarchd has been loaded" << endl;
	return 0;
}

void ModuleArchd::Uninit()
{
	cout << "module mdlarchd has been unloaded" << endl;
}

void ModuleArchd::service_processor(arch::ArchMessage& onode, const arch::ArchMessage& inode)
{
	cout << "mdlarchd: this is an echo protocol module" << endl;
	cout << "mdlarchd: processing a request ..." << endl;

	onode.set_conn_ctrl_type(arch::CCT_Close_AfterSend);

	const arch::ProtocolObjectArch* sobj = static_cast<const arch::ProtocolObjectArch*>(inode.get_data_object());
	arch::ProtocolObjectArch* dobj = new arch::ProtocolObjectArch();

	Message srcmsg;
	decode_msg(srcmsg, *sobj);


	onode.set_data_object(dobj);
	cout << "mdlarchd: respond a request" << endl;
}
