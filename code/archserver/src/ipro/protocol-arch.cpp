#include "protocol-arch.hpp"

using namespace core;

ipro::protocol_arch::ArchProtocolData::ArchProtocolData()
	: _version(APV_Unknown)
	, _header_ext_cache({ 0 })
	, _header_ext_cache_idx(0)
	, _parsing_phase(APP_Start)
	, _content_length(0)
	, _svc_id(0)
	, _svc_inst_id(0)
{}

ipro::protocol_arch::ArchProtocolData::~ArchProtocolData()
{}

core::IProtocolData::service_id_t
ipro::protocol_arch::ArchProtocolData::service_id() const noexcept
{
	return _svc_id;
}

core::IProtocolData::service_inst_id_t
ipro::protocol_arch::ArchProtocolData::service_inst_id() const noexcept
{
	return _svc_inst_id;
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

	switch (obj._parsing_phase)
	{
	case APP_Start:
		obj._parsing_phase = APP_Parsing_Header_Version;
		// continue to the next phase, no BREAK here!

	case APP_Parsing_Header_Version:
		assert(procbytes == 0);
		obj._version = readbuf[procbytes]; ++procbytes;
		obj._parsing_phase = APP_Parsing_Header_Extension;
		// continue to the next phase, no BREAK here!
		
	case APP_Parsing_Header_Extension:
		if (obj._version == APV_0_1)
		{ // APV v0.1
_GOTO_LAB_PROC_HEADER_EXT_V0_1:	// Ugly but simple way.
			if (procbytes < toreadlen)
			{
				switch (obj._header_ext_cache_idx)
				{
				case 0:
					obj._header_ext_cache[0] = readbuf[procbytes];
					++procbytes;
					++obj._header_ext_cache_idx;
					goto _GOTO_LAB_PROC_HEADER_EXT_V0_1;

				case 1:
					obj._header_ext_cache[1] = readbuf[procbytes];
					++procbytes;
					++obj._header_ext_cache_idx;

					obj._parsing_phase = APP_Parsing_Content;
					obj._content_length =
						obj._header_ext_cache[1] << 8 |
						obj._header_ext_cache[0];
					goto _GOTO_LAB_PROC_CONTENT;

				default:
					assert(false);
				}
			}
		}
		else
		{ // unknown arch protocol version
			return PPR_ERROR;
		}
		break;

	case APP_Parsing_Content:
_GOTO_LAB_PROC_CONTENT:
		{
			uint32_t canread = obj._content_length - (uint32_t)obj._data.size();
			if (canread > 0)
			{
				canread = (toreadlen - procbytes) < canread ? (uint32_t)(toreadlen - procbytes) : canread;
				obj._data.insert(obj._data.end(), readbuf + procbytes, readbuf + procbytes + canread);
				procbytes += canread;

				if (obj._data.size() == obj._content_length)
				{

					return PPR_PULSE;
				}
			}
			else
			{
				PPR_ERROR;
			}
		}
		break;
	}

	return PPR_AGAIN;
}


bool
ipro::protocol_arch::ArchProtocol::proc_ostrm(std::string& obuffer, const IProtocolData& src)
{
	const ArchProtocolData& obj = static_cast<const ArchProtocolData&>(src);

	if (obj._version == APV_0_1)
	{
		// pack version
		obuffer.push_back((uint8_t)obj._version);

		// pack content length
		uint16_t content_length = (uint16_t)obj._data.size();
		obuffer.push_back(content_length & 0xff);
		obuffer.push_back((content_length & 0xff00) >> 8);

		// pack content
		obuffer.insert(obuffer.end(), obj._data.begin(), obj._data.end());
		return true;
	}
	else
	{
		return false;
	}
}

bool
ipro::protocol_arch::ArchProtocol::proc_check_switch(ProtocolType& dest_proto, const IProtocolData& obj)
{
	return false;
}



