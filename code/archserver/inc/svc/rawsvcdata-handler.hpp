#pragma once

#include "pre-req.hpp"
#include "protocol-queue.hpp"

namespace svc
{

	class RawSvcDataHandler
	{
	public:
		bool deserialize(
			const std::uint8_t* data,
			size_t length);

		void get_deserialized(core::ProtocolQueue::node_t&);
		
		bool serialize(
			std::vector<uint8_t>& data,
			const core::ProtocolQueue::node_t& node);

	private:
		static uint8_t	_high8(uint16_t n);
		static uint8_t	_low8(uint16_t n);

	private:
		core::ProtocolQueue::node_t	temp;
	};

}






