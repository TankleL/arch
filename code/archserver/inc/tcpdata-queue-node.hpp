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
	class TCPDataQueueNode :
		public IAcquriable<TCPDataQueueNode>
	{
	public:
		TCPDataQueueNode();
		TCPDataQueueNode(TCPDataQueueNode&& rhs) noexcept;
		TCPDataQueueNode(IServiceDataObject* dobj, link_handle_t hlink, uint32_t uid, ConnCtrlType cct = CCT_None) noexcept;
		~TCPDataQueueNode();

		TCPDataQueueNode& operator=(TCPDataQueueNode&& rhs) noexcept;

	public:
		TCPDataQueueNode(const TCPDataQueueNode&) = delete;
		TCPDataQueueNode& operator=(const TCPDataQueueNode&) = delete;

	public:
		TCPDataQueueNode& acquire(TCPDataQueueNode& node) noexcept override;

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



