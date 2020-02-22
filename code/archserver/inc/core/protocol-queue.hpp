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
			{}

			_node(
				const std::weak_ptr<IProtocolData>& protocol_data,
				uint16_t connection_id)
				: data(protocol_data)
				, conn_id(connection_id)
			{}

			_node(_node&& rhs) noexcept
				: data(std::move(rhs.data))
				, conn_id(rhs.conn_id)
			{}

			_node& operator=(_node&& rhs) noexcept
			{
				data = std::move(rhs.data);
				conn_id = rhs.conn_id;
				return *this;
			}

			_node(const _node& rhs) = delete;
			_node& operator=(const _node& rhs) = delete;

			std::weak_ptr<IProtocolData>	data;
			std::shared_ptr<IProtocolData>	sdata;
			uint16_t						conn_id;
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

