#pragma once

#include "archserver-prereq.hpp"
#include "protocols.hpp"
#include "protocolproc.hpp"

namespace arch
{
	/******************************************************************/
	/*                        Arch Message Header                     */
	/*                                                                */
	/*     # V0.1                                                     */
	/*         - [HEADER] Version -> 1 byte                           */
	/*         - [HEADER] Extension                                   */
	/*         -       * [EXT] length -> 2 bytes                      */
	/*         - [CONTENT] Data -> length byte(s)                     */
	/*                                                                */
	/******************************************************************/

	enum ArchParsingPhase : int
	{
		APP_Start = 0,
		APP_Parsing_Header_Version,
		APP_Parsing_Header_Extension,
		APP_Parsing_Content
	};

	class Internal_ProtoObjectArch;
	class ProtoProcArch : public IProtocolProc
	{
	public:
		virtual ProtoProcRet proc_istrm(IProtocolObject& dest, cbyte_ptr readbuf, ssize_t toreadlen, ssize_t& procbytes) override;
		virtual bool proc_ostrm(std::string& obuffer, const IProtocolObject& src) override;
		virtual bool proc_check_switch(ProtocolType& dest_proto, const IProtocolObject& obj) override;
	};


	class Internal_ProtoObjectArch
		: public ProtocolObjectArch
	{
		friend ProtoProcArch;
	public:
		Internal_ProtoObjectArch(const Internal_ProtoObjectArch&) = delete;
		Internal_ProtoObjectArch& operator=(const Internal_ProtoObjectArch& rhs) = delete;

	public:
		Internal_ProtoObjectArch();
		Internal_ProtoObjectArch(Internal_ProtoObjectArch&& rhs) noexcept;
		virtual ~Internal_ProtoObjectArch();

		Internal_ProtoObjectArch& operator=(Internal_ProtoObjectArch&& rhs) noexcept;

	public:
		virtual IProtocolObject& acquire(IProtocolObject& src) noexcept override;
		virtual void dispose() noexcept override;

	private:
		std::array<byte_t, 4>	_header_ext_cache;
		uint32_t				_header_ext_cache_idx;
		ArchParsingPhase		_parsing_phase;
		uint32_t				_content_length;
	};
}

