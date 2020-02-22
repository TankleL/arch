#include "asvc-rawsvcdata-handler.hpp"

using namespace archsvc;

archsvc::RawSvcDataHandler::RawSvcDataHandler()
	: _pp(PP_Idle)
{}

archsvc::RawSvcDataHandler::~RawSvcDataHandler()
{}


archsvc::RawSvcDataHandler::ParsingPhase
archsvc::RawSvcDataHandler::deserialize(
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
		case PP_ConnIDH8:
			_conn_id.value = 0;
			_conn_id.high = *(data + procbytes++);
			_pp = PP_ConnIDL8;
			break;

		case PP_ConnIDL8:
			_conn_id.low = *(data + procbytes++);
			_pp = PP_High8;
			break;

		case PP_High8:
			_len.value = 0;
			_len.high = *(data + procbytes++);
			_pp = PP_Low8;
			break;

		case PP_Low8:
			_len.low = *(data + procbytes++);
			_pp = PP_Data;
			break;

		case PP_Data:
		{
			size_t canread =
				(length - procbytes) < _len.value?
				(length - procbytes) :
				_len.value;

			_temp.insert(
				_temp.end(),
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

void archsvc::RawSvcDataHandler::get_deserialized(
	std::vector<uint8_t>& data)
{
	data = std::move(_temp);
	_temp = std::move(std::vector<uint8_t>());
}

void archsvc::RawSvcDataHandler::serialiaze(
	std::vector<uint8_t>& odata,
	const uint16_t& conn_id,
	const std::vector<uint8_t>& idata)
{
	uint16_t datasize = (uint16_t)idata.size();

	/* *************************************
	 * [Data 0]    conn id
	 * [Size]      2 byte 
	 ************************************* */
	odata.push_back(_high8(conn_id));
	odata.push_back(_low8(conn_id));

	/* *************************************
	 * [Data 0]    protocol data size
	 * [Size]      2 byte 
	 ************************************* */
	odata.push_back(_high8(datasize));
	odata.push_back(_low8(datasize));

	/* *************************************
	 * [Data 0]    protocol data
	 * [Size]      data size
	 ************************************* */
	odata.insert(
		odata.end(),
		idata.begin(),
		idata.end());
}

std::uint8_t
archsvc::RawSvcDataHandler::_high8(uint16_t n)
{
	return n >> 8;
}

std::uint8_t
archsvc::RawSvcDataHandler::_low8(uint16_t n)
{
	return n & 0xff;
}



