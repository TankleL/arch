#include "protocol-arch.hpp"

using namespace core;

ipro::protocol_arch::ArchProtocolData::ArchProtocolData()
	: _version(APV_Unknown)
	, _parsing_phase(APP_Start)
	, _content_length(0)
	, _conn_id(0)
{}

ipro::protocol_arch::ArchProtocolData::~ArchProtocolData()
{}

core::IProtocolData::service_id_t
ipro::protocol_arch::ArchProtocolData::service_id() const noexcept
{
	return _svc_id.value();
}

core::IProtocolData::service_inst_id_t
ipro::protocol_arch::ArchProtocolData::service_inst_id() const noexcept
{
	return _svc_inst_id.value();
}

void
ipro::protocol_arch::ArchProtocolData::set_service_inst_id(service_inst_id_t id) noexcept
{
	_svc_inst_id.value(id);
}

core::IProtocolData::length_t
ipro::protocol_arch::ArchProtocolData::length() const noexcept
{
	return (length_t)_data.size();
}

const std::uint8_t*
ipro::protocol_arch::ArchProtocolData::data() const noexcept
{
	return _data.data();
}


core::ProtocolType
ipro::protocol_arch::ArchProtocol::get_protocol_type() const noexcept
{
	return PT_Arch;
}


core::IProtocolHandler::ProtoProcRet
ipro::protocol_arch::ArchProtocol::proc_istrm(
	IProtocolData& dest, std::uint8_t* readbuf, size_t toreadlen, size_t& procbytes)
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
ipro::protocol_arch::ArchProtocol::proc_ostrm(
	std::vector<uint8_t>& obuffer,
	const IProtocolData& rsp,
	const IProtocolData& req)
{
	bool result = true;
	const core::PlainProtocolData& rspobj = static_cast<const core::PlainProtocolData&>(rsp);
	const ArchProtocolData& reqobj = static_cast<const ArchProtocolData&>(req);

	if (reqobj._version.value() == APV_0_1) // check proto version
	{
		VUInt uinthelper;

		// pack version
		uinthelper.value(APV_0_1);
		obuffer << uinthelper;

		// pack svc id
		obuffer << reqobj._svc_id;

		// pack svc inst id
		obuffer << reqobj._svc_inst_id;

		// pack content length
		uinthelper.value(rspobj.length());
		obuffer << uinthelper;

		// pack content
		obuffer.insert(
			obuffer.end(),
			rspobj.protodata.begin(),
			rspobj.protodata.end());
	}
	else
	{
		result = false;
	}

	return result;
}

bool
ipro::protocol_arch::ArchProtocol::proc_check_switch(ProtocolType& dest_proto, const IProtocolData& obj)
{
	return false;
}



