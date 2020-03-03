#pragma once
#include "pre-req.hpp"
#include "protocol.hpp"

namespace core
{

	class ProtocolQueue
	{
	public:
		typedef struct _node
		{
			_node() noexcept
				: conn_id(0)
				, ccf(0)
				, proto(archproto::PT_Unknown)
			{}

			_node(
				std::unique_ptr<archproto::IProtocolData>&& protocol_data,
				uint16_t connection_id,
				uint16_t conn_ctrl_flags,
				archproto::ProtocolType protocol)
				: data(std::move(protocol_data))
				, conn_id(connection_id)
				, ccf(conn_ctrl_flags)
				, proto(protocol)
			{}

			_node(_node&& rhs) noexcept
				: data(std::move(rhs.data))
				, conn_id(std::move(rhs.conn_id))
				, ccf(std::move(rhs.ccf))
				, proto(std::move(rhs.proto))
			{}

			_node& operator=(_node&& rhs) noexcept
			{
				data = std::move(rhs.data);
				conn_id = std::move(rhs.conn_id);
				ccf = std::move(rhs.ccf);
				proto = std::move(rhs.proto);
				return *this;
			}

			_node(const _node& rhs) = delete;
			_node& operator=(const _node& rhs) = delete;

			std::unique_ptr<archproto::IProtocolData>	data;
			uint16_t									conn_id;
			uint16_t									ccf;	// connection control flags
			archproto::ProtocolType						proto;
		} node_t;

	public:
		ProtocolQueue() noexcept;
		ProtocolQueue(ProtocolQueue&& rhs) noexcept;
		ProtocolQueue& operator=(ProtocolQueue&& rhs) noexcept;
		~ProtocolQueue() noexcept;

		ProtocolQueue(const ProtocolQueue& rhs) = delete;
		ProtocolQueue& operator=(const ProtocolQueue& rhs) = delete;

	public:
		void push(node_t&& node);
		bool pop(node_t& node);
		bool pop(std::vector<node_t>& results);
		size_t size() const noexcept;

	private:
		std::deque<node_t>	_queue;
		std::mutex			_mtx_push_pop;
	};


}

