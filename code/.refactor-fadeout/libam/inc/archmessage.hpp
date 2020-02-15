#pragma once
#include <cstdint>
#include <protocols.hpp>

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

	class ArchMessage : public IAcquriable<ArchMessage>
	{
	public:
		ArchMessage();
		ArchMessage(ArchMessage&& rhs) noexcept;
		ArchMessage(IProtocolObject* dobj, link_handle_t hlink, uint32_t uid, ConnCtrlType cct = CCT_None) noexcept;
		~ArchMessage();

		ArchMessage& operator=(ArchMessage&& rhs) noexcept;

	public:
		ArchMessage(const ArchMessage&) = delete;
		ArchMessage& operator=(const ArchMessage&) = delete;

		virtual ArchMessage& acquire(ArchMessage& src) noexcept override;

	public:
		void					set_data_object(IProtocolObject* dobj) noexcept;
		void					set_conn_ctrl_type(ConnCtrlType cct) noexcept;

		IProtocolObject*		get_data_object() const noexcept;
		link_handle_t			get_hlink() const noexcept;
		uint32_t				get_uid() const noexcept;
		ConnCtrlType			get_conn_ctrl_type() const noexcept;
		void					clear();

	protected:
		IProtocolObject*	_dobj;
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

	inline ArchMessage::ArchMessage(IProtocolObject* dobj, link_handle_t hlink, uint32_t uid, ConnCtrlType cct) noexcept
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
		return acquire(rhs);
	}

	inline ArchMessage& ArchMessage::acquire(ArchMessage& src) noexcept
	{
		_dobj = src._dobj;				src._dobj = nullptr;
		_hlink = src._hlink;			src._hlink = nullptr;
		_uid = src._uid;				src._uid = 0;
		_cct = src._cct;				src._cct = CCT_None;
		return *this;
	}

	inline void ArchMessage::set_data_object(IProtocolObject* dobj) noexcept
	{
		_dobj = dobj;
	}

	inline void ArchMessage::set_conn_ctrl_type(ConnCtrlType cct) noexcept
	{
		_cct = cct;
	}

	inline IProtocolObject* ArchMessage::get_data_object() const noexcept
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



