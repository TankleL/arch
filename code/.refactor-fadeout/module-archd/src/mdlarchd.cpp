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

void encode_msg(arch::ProtocolObjectArch& dobj, const Message& msg)
{
	dobj.version = APV_0_1;

	// encode evid
	int evid = MessageUtil::get_event_id(msg.get_id());
	if (evid < 0x80) // 0 ~ 7 bits
	{
		// [byte 0] 0xxx xxxx
		dobj.data.push_back(evid);
	}
	else if (evid < 0x4000) // 8 ~ 14 bits
	{
		// [byte 0] 10xx xxxx
		// [byte 1] xxxx xxxx

		dobj.data.push_back(0x80 | (evid & 0x3f));
		dobj.data.push_back(evid >> 6);
	}
	else if (evid < 0x200000) // 15 ~ 21 bits
	{
		// [byte 0] 110x xxxx
		// [byte 1] xxxx xxxx
		// [byte 2] xxxx xxxx

		dobj.data.push_back(0xc0 | (evid & 0x1f));
		dobj.data.push_back((evid & 0x1fe0) >> 5);
		dobj.data.push_back(evid >> 13);
	}
	else // 22 ~ 24 bits
	{
		// [byte 0] 1110 0000
		// [byte 1] xxxx xxxx
		// [byte 2] xxxx xxxx
		// [byte 3] xxxx xxxx

		dobj.data.push_back(0xe0);
		dobj.data.push_back(evid & 0xff);
		dobj.data.push_back(evid & 0xff00 >> 8);
		dobj.data.push_back(evid & 0xff0000 >> 16);
	}
	dobj.data.insert(dobj.data.end(), msg.get_data(), msg.get_data() + msg.get_length());
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

	// echo server.

	encode_msg(*dobj, srcmsg);
	onode.set_data_object(dobj);
	cout << "mdlarchd: respond a request" << endl;
}
