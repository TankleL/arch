#include "archmessage.hpp"
#include "tcpserver.hpp"

using namespace std;
using namespace arch;

ArchMessage::ArchMessage()
	: _dobj(nullptr)
	, _hlink(Invalid_linke_handle)
	, _uid(0)
	, _cct(CCT_None)
{}

ArchMessage::ArchMessage(ArchMessage&& rhs) noexcept
	: _dobj(rhs._dobj)
	, _hlink(rhs._hlink)
	, _uid(rhs._uid)
	, _cct(rhs._cct)
{
	rhs._dobj = nullptr;
	rhs._hlink = nullptr;
	rhs._uid = 0;
	rhs._cct = CCT_None;
}

ArchMessage::ArchMessage(IServiceDataObject* dobj, link_handle_t hlink, uint32_t uid, ConnCtrlType cct) noexcept
	: _dobj(dobj)
	, _hlink(hlink)
	, _uid(uid)
	, _cct(cct)
{}

ArchMessage::~ArchMessage()
{
	clear();
}

ArchMessage& ArchMessage::operator=(ArchMessage&& rhs) noexcept
{
	_dobj = rhs._dobj;				rhs._dobj = nullptr;
	_hlink = rhs._hlink;			rhs._hlink = nullptr;
	_uid = rhs._uid;				rhs._uid = 0;
	_cct = rhs._cct;				rhs._cct = CCT_None;
	return *this;
}

ArchMessage& ArchMessage::acquire(ArchMessage& node) noexcept
{
	_dobj = node._dobj; node._dobj = nullptr;
	_hlink = node._hlink;
	_uid = node._uid;
	_cct = node._cct;
	return *this;
}

IServiceDataObject* ArchMessage::get_data_object() const noexcept
{
	return _dobj;
}

link_handle_t ArchMessage::get_hlink() const noexcept
{
	return _hlink;
}

uint32_t ArchMessage::get_uid() const noexcept
{
	return _uid;
}

ConnCtrlType ArchMessage::get_conn_ctrl_type() const noexcept
{
	return _cct;
}

void ArchMessage::clear()
{
	if (_dobj)
	{
		_dobj->dispose(); _dobj = nullptr;
	}
	_hlink = Invalid_linke_handle;
	_uid = 0;
}