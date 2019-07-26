#pragma once
#include <cstdint>
#include <service-dataobj.hpp>

namespace arch
{

	typedef void*	link_handle_t;
	static const link_handle_t	Invalid_linke_handle = (link_handle_t)0;

	enum ConnCtrlType : int
	{
		CCT_None	= 0,
		CCT_Close_AfterSend,
		CCT_Close_Immediate,
		CCT_Switch_AfterSend_ToWebSocket,
	};

	class ArchMessage
	{
	public:
		ArchMessage();
		ArchMessage(ArchMessage&& rhs) noexcept;
		ArchMessage(IServiceDataObject* dobj, link_handle_t hlink, uint32_t uid, ConnCtrlType cct = CCT_None) noexcept;
		~ArchMessage();

		ArchMessage& operator=(ArchMessage&& rhs) noexcept;

	public:
		ArchMessage(const ArchMessage&) = delete;
		ArchMessage& operator=(const ArchMessage&) = delete;

	public:
		IServiceDataObject*		get_data_object() const noexcept;
		link_handle_t			get_hlink() const noexcept;
		uint32_t				get_uid() const noexcept;
		ConnCtrlType			get_conn_ctrl_type() const noexcept;
		void					clear();

	protected:
		IServiceDataObject*	_dobj;
		link_handle_t		_hlink;
		uint32_t			_uid;
		ConnCtrlType		_cct;
	};


	inline ArchMessage::ArchMessage()
		: _dobj(nullptr)
		, _hlink(Invalid_linke_handle)
		, _uid(0)
		, _cct(CCT_None)
	{}

	inline ArchMessage::ArchMessage(ArchMessage&& rhs) noexcept
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

	inline ArchMessage::ArchMessage(IServiceDataObject* dobj, link_handle_t hlink, uint32_t uid, ConnCtrlType cct) noexcept
		: _dobj(dobj)
		, _hlink(hlink)
		, _uid(uid)
		, _cct(cct)
	{}

	inline ArchMessage::~ArchMessage()
	{
		clear();
	}

	inline ArchMessage& ArchMessage::operator=(ArchMessage&& rhs) noexcept
	{
		_dobj = rhs._dobj;				rhs._dobj = nullptr;
		_hlink = rhs._hlink;			rhs._hlink = nullptr;
		_uid = rhs._uid;				rhs._uid = 0;
		_cct = rhs._cct;				rhs._cct = CCT_None;
		return *this;
	}

	inline IServiceDataObject* ArchMessage::get_data_object() const noexcept
	{
		return _dobj;
	}

	inline link_handle_t ArchMessage::get_hlink() const noexcept
	{
		return _hlink;
	}

	inline uint32_t ArchMessage::get_uid() const noexcept
	{
		return _uid;
	}

	inline ConnCtrlType ArchMessage::get_conn_ctrl_type() const noexcept
	{
		return _cct;
	}

	inline void ArchMessage::clear()
	{
		if (_dobj)
		{
			_dobj->dispose(); _dobj = nullptr;
		}
		_hlink = Invalid_linke_handle;
		_uid = 0;
	}
}



