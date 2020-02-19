#include "rawsvcdata-handler.hpp"

using namespace svc;
using namespace core;

bool svc::RawSvcDataHandler::deserialize(
	const std::uint8_t* data,
	size_t length)
{


	return false;
}

void svc::RawSvcDataHandler::get_deserialized(
	core::ProtocolQueue::node_t& node)
{
	node = std::move(temp);
	temp = std::move(core::ProtocolQueue::node_t());
}

bool svc::RawSvcDataHandler::serialize(
	std::vector<uint8_t>& data,
	const core::ProtocolQueue::node_t& node)
{
	bool result = true;
	auto protodata = node.data.lock();
	if (protodata)
	{
		/* *************************************
		 * [Header 0]  serializer verison
		 * [Size]      1 byte
		 ************************************* */
		data.push_back(1);

		/* *************************************
		 * [Data 0]    protocol data
		 * [Size]      data size
		 ************************************* */
		auto datasize = protodata->length();
		data.push_back(_high8(datasize));
		data.push_back(_low8(datasize));
		data.insert(
			data.end(),
			protodata->data(),
			protodata->data() + datasize);

		/* *************************************
		 * [Tail 0]    end of node
		 * [Size]      1 byte
		 ************************************* */
		data.push_back(0);
	}
	else
	{
		result = false;
	}

	return result;
}

std::uint8_t
svc::RawSvcDataHandler::_high8(uint16_t n)
{
	return n >> 8;
}

std::uint8_t
svc::RawSvcDataHandler::_low8(uint16_t n)
{
	return n & 0xff;
}






