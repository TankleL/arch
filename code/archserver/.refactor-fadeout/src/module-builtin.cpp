#include "module-builtin.hpp"
#include "modulemgr.hpp"

using namespace std;
using namespace arch;

BuiltinModule_WebSocket::Header::Header() noexcept
	: dw0(0)
{}

bool BuiltinModule_WebSocket::Header::get_fin() const
{
	return (ctrl_b0 & 0x80) != 0;
}

int BuiltinModule_WebSocket::Header::get_version() const
{
	return (ctrl_b0 & 0x70) >> 4;
}

bool BuiltinModule_WebSocket::Header::get_zip() const
{
	return (ctrl_b0 & 0x08) != 0;
}

void BuiltinModule_WebSocket::Header::set_fin(bool fin)
{
	ctrl_b0 &= 0x7f;
	if (fin)
	{
		ctrl_b0 |= 0x80;
	}
}

void BuiltinModule_WebSocket::Header::set_version(int version)
{ // 3 bits for version
	version &= 0x07;
	ctrl_b0 &= 0x8f;

	ctrl_b0 |= version << 4;
}

void BuiltinModule_WebSocket::Header::set_zip(bool zip)
{
	ctrl_b0 &= 0xf7;
	if (zip)
	{
		ctrl_b0 |= 0x08;
	}
}

void BuiltinModule_WebSocket::Header::digest(const byte_t* data)
{
	memcpy(&dw0, data, 4);
}

BuiltinModule_WebSocket::BuiltinModule_WebSocket(const ModuleManager* mgr) noexcept
	: _mdl_mgr(mgr)
{}

bool BuiltinModule_WebSocket::init()
{
	return true;
}

void BuiltinModule_WebSocket::uninit()
{

}

void BuiltinModule_WebSocket::process(ArchMessage& onode, const ArchMessage& inode)
{
	ProtocolObjectWebSocket* obj = static_cast<ProtocolObjectWebSocket *>(inode.get_data_object());
	if (obj->_ioframes.size() > 0 && obj->_ioframes.front().data.size() > 4)
	{
		header_t hd;
		hd.digest(obj->_ioframes.front().data.data());
		obj->_header_offset = 4;

		if (hd.get_fin() && hd.get_version() == 0)
		{ // version 0. multi-frames is not supported.
			IModule* mdl = _mdl_mgr->get_module(hd.proto_code);
			if (mdl)
			{
				mdl->process(onode, inode);
			}
		}
	}
}

void BuiltinModule_WebSocket::dispose() noexcept
{
	delete this;
}




bool BuiltinModule_Http::init()
{
	return true;
}

void BuiltinModule_Http::uninit()
{

}

void BuiltinModule_Http::process(ArchMessage& onode, const ArchMessage& inode)
{

}

void BuiltinModule_Http::dispose() noexcept
{

}

