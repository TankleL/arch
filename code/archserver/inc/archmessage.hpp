#pragma once

#include "archserver-prereq.hpp"
#include "acquirable.hpp"

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

	class IServiceDataObject;
	class ArchMessage :
		public IAcquriable<ArchMessage>
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
		ArchMessage& acquire(ArchMessage& node) noexcept override;

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

}



