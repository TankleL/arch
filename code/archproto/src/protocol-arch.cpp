#include "protocol-arch.hpp"
#include "vardata-utilities.hpp"

using namespace archproto;


ArchProtocolData::ArchProtocolData()
	: _version(APV_Unknown)
	, _parsing_phase(APP_Start)
	, _content_length(0)
{}

ArchProtocolData::~ArchProtocolData()
{}

IProtocolData::service_id_t
ArchProtocolData::service_id() const noexcept
{
	return _svc_id.value();
}

IProtocolData::service_inst_id_t
ArchProtocolData::service_inst_id() const noexcept
{
	return _svc_inst_id.value();
}

void
ArchProtocolData::set_service_id(service_id_t id) noexcept
{
	_svc_id.value(id);
}

void
ArchProtocolData::set_service_inst_id(service_inst_id_t id) noexcept
{
	_svc_inst_id.value(id);
}

IProtocolData::length_t
ArchProtocolData::length() const noexcept
{
	return (length_t)_data.size();
}

const std::uint8_t*
ArchProtocolData::data() const noexcept
{
	return _data.data();
}


ProtocolType
ArchProtocol::get_protocol_type() const noexcept
{
	return PT_Arch;
}


IProtocolHandler::ProtoProcRet
ArchProtocol::des_sock_stream(
	IProtocolData& dest,
	const uint8_t* readbuf,
	size_t toreadlen,
	size_t& procbytes)
{
	procbytes = 0;
	ArchProtocolData& obj = static_cast<ArchProtocolData&>(dest);

	bool goon = true;
	VUInt::DigestStatus digst;
	while ((toreadlen - procbytes > 0) && goon)
	{
		switch (obj._parsing_phase)
		{
		case APP_Start:
			obj._parsing_phase = APP_Parsing_Header_Version;
			// continue to the next phase, no BREAK here!

		case APP_Parsing_Header_Version:
			digst = obj._version.digest(readbuf[procbytes++]);
			if (VUInt::DS_Idle == digst)
			{
				if (obj._version.value() != APV_0_1)
				{ // unsupported protocol version
					return PPR_ERROR;
				}
				obj._parsing_phase = APP_Parsing_Header_SVC_ID;
			}
			else if (VUInt::DS_Bad == digst)
			{
				return PPR_ERROR;
			}

			break;

		case APP_Parsing_Header_SVC_ID:
			digst = obj._svc_id.digest(readbuf[procbytes++]);
			if (VUInt::DS_Idle == digst)
			{
				obj._parsing_phase = APP_Parsing_Header_SVC_INST_ID;
			}
			else if (VUInt::DS_Bad == digst)
			{
				return PPR_ERROR;
			}
			break;

		case APP_Parsing_Header_SVC_INST_ID:
			digst = obj._svc_inst_id.digest(readbuf[procbytes++]);
			if (VUInt::DS_Idle == digst)
			{
				obj._parsing_phase = APP_Parsing_Header_Length;
			}
			else if (VUInt::DS_Bad == digst)
			{
				return PPR_ERROR;
			}
			break;

		case APP_Parsing_Header_Length:
			digst = obj._content_length.digest(readbuf[procbytes++]);
			if (VUInt::DS_Idle == digst)
			{
				if (obj._content_length.value() > 65536)
				{ // max content leng is 65536 bytes
					return PPR_ERROR;
				}
				obj._parsing_phase = APP_Parsing_Content;
			}
			else if (VUInt::DS_Bad == digst)
			{
				return PPR_ERROR;
			}
			break;

		case APP_Parsing_Content:
		{
			uint32_t canread = obj._content_length.value() - (uint32_t)obj._data.size();
			if (canread > 0)
			{
				canread = (toreadlen - procbytes) < canread ? (uint32_t)(toreadlen - procbytes) : canread;
				obj._data.insert(obj._data.end(), readbuf + procbytes, readbuf + procbytes + canread);
				procbytes += canread;

				if ((uint32_t)obj._data.size() == obj._content_length.value())
				{
					return PPR_PULSE;
				}
			}
			else
			{
				return PPR_ERROR;
			}
		}
			break;
		}
	}

	return PPR_AGAIN;
}


bool
ArchProtocol::ser_sock_stream(
	std::vector<uint8_t>& dest,
	const IProtocolData& obj)
{
	bool result = true;
	const ArchProtocolData& src = static_cast<const ArchProtocolData&>(obj);

	if (src._version.value() == APV_0_1) // check proto version
	{
		// pack version
		dest << VUInt(APV_0_1);

		// pack svc id
		dest << src._svc_id;

		// pack svc inst id
		dest << src._svc_inst_id;

		// pack content length
		dest << VUInt(src.length());

		// pack content
		dest << src._data;
	}
	else
	{
		result = false;
	}

	return result;
}

bool
ArchProtocol::chk_sock_protoswitch(
	ProtocolType& dest_proto,
	const IProtocolData& obj)
{
	return false;
}

IProtocolHandler::ProtoProcRet
ArchProtocol::des_service_stream(
	IProtocolData& dest,
	const uint8_t* src,
	size_t toreadlen,
	size_t& procbytes)
{
	procbytes = 0;
	ArchProtocolData& obj = static_cast<ArchProtocolData&>(dest);

	bool goon = true;
	VUInt::DigestStatus digst;
	while ((toreadlen - procbytes > 0) && goon)
	{
		switch (obj._parsing_phase)
		{
		case APP_Start:
			obj._parsing_phase = APP_Parsing_Header_Version;
			// continue to the next phase, no BREAK here!

		case APP_Parsing_Header_Version:
			digst = obj._version.digest(src[procbytes++]);
			if (VUInt::DS_Idle == digst)
			{
				if (obj._version.value() != APV_0_1)
				{ // unsupported protocol version
					return PPR_ERROR;
				}
				obj._parsing_phase = APP_Parsing_Header_SVC_ID;
			}
			else if (VUInt::DS_Bad == digst)
			{
				return PPR_ERROR;
			}

			break;

		case APP_Parsing_Header_SVC_ID:
			digst = obj._svc_id.digest(src[procbytes++]);
			if (VUInt::DS_Idle == digst)
			{
				obj._parsing_phase = APP_Parsing_Header_SVC_INST_ID;
			}
			else if (VUInt::DS_Bad == digst)
			{
				return PPR_ERROR;
			}
			break;

		case APP_Parsing_Header_SVC_INST_ID:
			digst = obj._svc_inst_id.digest(src[procbytes++]);
			if (VUInt::DS_Idle == digst)
			{
				obj._parsing_phase = APP_Parsing_Header_Length;
			}
			else if (VUInt::DS_Bad == digst)
			{
				return PPR_ERROR;
			}
			break;

		case APP_Parsing_Header_Length:
			digst = obj._content_length.digest(src[procbytes++]);
			if (VUInt::DS_Idle == digst)
			{
				if (obj._content_length.value() > 65536)
				{ // max content leng is 65536 bytes
					return PPR_ERROR;
				}
				obj._parsing_phase = APP_Parsing_Content;
			}
			else if (VUInt::DS_Bad == digst)
			{
				return PPR_ERROR;
			}
			break;

		case APP_Parsing_Content:
		{
			uint32_t canread = obj._content_length.value() - (uint32_t)obj._data.size();
			if (canread > 0)
			{
				canread = (toreadlen - procbytes) < canread ? (uint32_t)(toreadlen - procbytes) : canread;
				obj._data.insert(obj._data.end(), src + procbytes, src + procbytes + canread);
				procbytes += canread;

				if ((uint32_t)obj._data.size() == obj._content_length.value())
				{
					return PPR_PULSE;
				}
			}
			else
			{
				return PPR_ERROR;
			}
		}
			break;
		}
	}

	return PPR_AGAIN;
	return PPR_AGAIN;
}

bool
ArchProtocol::ser_service_stream(
	std::vector<uint8_t>& dest,
	const IProtocolData& obj)
{
	const ArchProtocolData& src = static_cast<const ArchProtocolData&>(obj);

	dest << src._version;
	dest << src._svc_id;
	dest << src._svc_inst_id;
	dest << src._content_length;
	dest << src._data;

	return true;
}








