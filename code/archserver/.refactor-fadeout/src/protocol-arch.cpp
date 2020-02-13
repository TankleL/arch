#include "protocol-arch.hpp"

using namespace std;
using namespace arch;

ProtoProcRet ProtoProcArch::proc_istrm(IProtocolObject& dest, cbyte_ptr readbuf, ssize_t toreadlen, ssize_t& procbytes)
{
	procbytes = 0;
	Internal_ProtoObjectArch& obj = static_cast<Internal_ProtoObjectArch&>(dest);

	switch (obj._parsing_phase)
	{
	case APP_Start:
		obj._parsing_phase = APP_Parsing_Header_Version;
		// continue to the next phase, no BREAK here!

	case APP_Parsing_Header_Version:
		assert(procbytes == 0);
		obj.version = readbuf[procbytes]; ++procbytes;
		obj._parsing_phase = APP_Parsing_Header_Extension;
		// continue to the next phase, no BREAK here!
		
	case APP_Parsing_Header_Extension:
		if (obj.version == APV_0_1)
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
			uint32_t canread = obj._content_length - (uint32_t)obj.data.size();
			if (canread > 0)
			{
				canread = (toreadlen - procbytes) < canread ? (uint32_t)(toreadlen - procbytes) : canread;
				obj.data.insert(obj.data.end(), readbuf + procbytes, readbuf + procbytes + canread);
				procbytes += canread;

				if (obj.data.size() == obj._content_length)
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


bool ProtoProcArch::proc_ostrm(std::string& obuffer, const IProtocolObject& src)
{
	const Internal_ProtoObjectArch& obj = static_cast<const Internal_ProtoObjectArch&>(src);
	
	if (obj.version == APV_0_1)
	{
		// pack version
		obuffer.push_back((uint8_t)obj.version);

		// pack content length
		uint16_t content_length = (uint16_t)obj.data.size();
		obuffer.push_back(content_length & 0xff);
		obuffer.push_back((content_length & 0xff00) >> 8);

		// pack content
		obuffer.insert(obuffer.end(), obj.data.begin(), obj.data.end());

		return true;
	}
	else
	{
		return false;
	}
}


bool ProtoProcArch::proc_check_switch(ProtocolType& dest_proto, const IProtocolObject& obj)
{
	// so far, this protocol DOES NOT support protocol switching.
	return false;
}



Internal_ProtoObjectArch::Internal_ProtoObjectArch()
	: _header_ext_cache({ 0 })
	, _header_ext_cache_idx(0)
	, _parsing_phase(APP_Start)
	, _content_length(0)
{}

Internal_ProtoObjectArch::Internal_ProtoObjectArch(Internal_ProtoObjectArch&& rhs) noexcept
	: ProtocolObjectArch(std::move(rhs))
	, _header_ext_cache(std::move(rhs._header_ext_cache))
	, _header_ext_cache_idx(0)
	, _parsing_phase(rhs._parsing_phase)
	, _content_length(rhs._content_length)
{}

Internal_ProtoObjectArch& Internal_ProtoObjectArch::operator=(Internal_ProtoObjectArch&& rhs) noexcept
{
	ProtocolObjectArch::operator=(std::move(rhs));
	_header_ext_cache = std::move(rhs._header_ext_cache);
	_header_ext_cache_idx = rhs._header_ext_cache_idx;
	_parsing_phase = rhs._parsing_phase;
	_content_length = rhs._content_length;
	return *this;
}

Internal_ProtoObjectArch::~Internal_ProtoObjectArch()
{}

IProtocolObject& Internal_ProtoObjectArch::acquire(IProtocolObject& src) noexcept
{
#ifdef _DEBUG
	Internal_ProtoObjectArch& obj = dynamic_cast<Internal_ProtoObjectArch&>(src);
	assert(typeid(obj) == typeid(Internal_ProtoObjectArch));
#else
	ProtocolObjectArch& obj = static_cast<ProtocolObjectArch&>(src);
#endif // _DEBUG

	ProtocolObjectArch::acquire(src);
	_header_ext_cache = std::move(obj._header_ext_cache);
	_header_ext_cache_idx = obj._header_ext_cache_idx;
	_parsing_phase = obj._parsing_phase;
	_content_length = obj._content_length;
	return *this;
}

void Internal_ProtoObjectArch::dispose() noexcept
{
	delete this;
}

