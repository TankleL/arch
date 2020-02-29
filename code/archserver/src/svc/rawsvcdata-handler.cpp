#include "rawsvcdata-handler.hpp"
#include "protocol.hpp"

using namespace svc;
using namespace core;


svc::RawSvcDataHandler::RawSvcDataHandler()
	: _pp(PP_Idle)
{}

svc::RawSvcDataHandler::~RawSvcDataHandler()
{}

svc::RawSvcDataHandler::ParsingPhase svc::RawSvcDataHandler::deserialize(
	const std::uint8_t* data,
	size_t length,
	size_t& procbytes)
{
	procbytes = 0;

	bool goon = true;
	while (goon && procbytes < length)
	{
		switch (_pp)
		{
		case PP_Idle:
			_pp = PP_ConnIDH8;
			_temp = core::ProtocolQueue::node_t();
		case PP_ConnIDH8:
			_conn_id.value = 0;
			_conn_id.high = *(data + procbytes++);
			_pp = PP_ConnIDL8;
			break;

		case PP_ConnIDL8:
			_conn_id.low = *(data + procbytes++);
			_temp.conn_id = _conn_id.value;
			_pp = PP_CCFH8;
			break;

		case PP_CCFH8:
			_ccf.value = 0;
			_ccf.high = *(data + procbytes++);
			_pp = PP_CCFL8;
			break;

		case PP_CCFL8:
			_ccf.low = *(data + procbytes++);
			_temp.ccf = _ccf.value;
			_pp = PP_DataSizeH8;
			break;

		case PP_DataSizeH8:
			_len.high = *(data + procbytes++);
			_pp = PP_DataSizeL8;
			break;

		case PP_DataSizeL8:
			_len.low = *(data + procbytes++);
			_pp = PP_Data;
			break;

		case PP_Data:
		{
			size_t canread =
				(length - procbytes) < _len.value?
				(length - procbytes) :
				_len.value;

			PlainProtocolData& pdata = _ensure_sdata(_temp);
			pdata.protodata.insert(
				pdata.protodata.end(),
				data + procbytes,
				data + procbytes + canread);
			procbytes += canread;

			_pp = PP_Idle;
			goon = false;
		}
			break;
		}
	}

	return _pp;
}

void svc::RawSvcDataHandler::get_deserialized(
	core::ProtocolQueue::node_t& node)
{
	node = std::move(_temp);
}

bool svc::RawSvcDataHandler::serialize(
	std::vector<uint8_t>& data,
	const core::ProtocolQueue::node_t& node)
{
	bool result = true;
	auto protodata = node.data.lock();
	if (protodata)
	{
		auto datasize = protodata->length();

		/* *************************************
		 * [Data 0]    conn id
		 * [Size]      2 bytes 
		 ************************************* */
		data.push_back(_high8(node.conn_id));
		data.push_back(_low8(node.conn_id));

		/* *************************************
		 * [Data 0]    ccf
		 * [Size]      2 bytes 
		 ************************************* */
		data.push_back(_high8(node.ccf));
		data.push_back(_low8(node.ccf));

		/* *************************************
		 * [Data 0]    protocol data size
		 * [Size]      2 bytes 
		 ************************************* */
		data.push_back(_high8(datasize));
		data.push_back(_low8(datasize));

		/* *************************************
		 * [Data 0]    protocol data
		 * [Size]      data size
		 ************************************* */
		data.insert(
			data.end(),
			protodata->data(),
			protodata->data() + datasize);
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

core::PlainProtocolData&
svc::RawSvcDataHandler::_ensure_sdata(core::ProtocolQueue::node_t& node)
{
	if (nullptr == node.sdata)
	{
		node.sdata = std::make_shared<core::PlainProtocolData>();
	}
	return static_cast<PlainProtocolData&>(*(node.sdata));
}





