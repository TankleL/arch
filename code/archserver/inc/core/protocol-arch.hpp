#pragma once
#include "protocol.hpp"

namespace core
{

	namespace protocol_arch
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

		enum ArchProtocolVersion : int
		{
			APV_Unknown,
			APV_0_1		// arch protocol 0.1
		};

		enum ArchParsingPhase : int
		{
			APP_Start = 0,
			APP_Parsing_Header_Version,
			APP_Parsing_Header_Extension,
			APP_Parsing_Content
		};

		class ArchProtocol : public IProtocolHandler
		{
		public:
			virtual ProtocolType	get_protocol_type() const noexcept override;
			virtual ProtoProcRet	proc_istrm(std::any& dest, std::uint8_t* readbuf, size_t toreadlen, size_t& procbytes) override;
			virtual bool			proc_ostrm(std::string& obuffer, const std::any& src) override;
			virtual bool			proc_check_switch(ProtocolType& dest_proto, const std::any& obj) override;
		};

		class ArchProtocolData
		{
			friend ArchProtocol;
		private:
			std::vector<uint8_t>	_data;
			uint32_t				_version;
			std::array<uint8_t, 4>	_header_ext_cache;
			uint32_t				_header_ext_cache_idx;
			ArchParsingPhase		_parsing_phase;
			uint32_t				_content_length;
		};

	}

}






