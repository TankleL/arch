#pragma once
#include "protocol.hpp"

namespace ipro
{

	namespace protocol_arch
	{

		/******************************************************************/
		/*                        Arch Message Header                     */
		/*                                                                */
		/*     # V0.1                                                     */
		/*         - [HEADER] Version      -> 1 byte                      */
		/*         - [HEADER] length       -> 2 bytes			          */
		/*         - [HEADER] svc_inst_id  -> 2 bytes                     */
		/*         - [CONTENT] Data        -> length byte(s)              */
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
			APP_Parsing_Header_Length,
			APP_Parsing_Header_SVCINSTID,
			APP_Parsing_Content
		};

		class ArchProtocol : public core::IProtocolHandler
		{
		public:
			virtual core::ProtocolType	get_protocol_type() const noexcept override;
			virtual ProtoProcRet	proc_istrm(core::IProtocolData& dest, std::uint8_t* readbuf, size_t toreadlen, size_t& procbytes) override;
			virtual bool			proc_ostrm(std::string& obuffer, const core::IProtocolData& src) override;
			virtual bool			proc_check_switch(core::ProtocolType& dest_proto, const core::IProtocolData& obj) override;
		};

		class ArchProtocolData : public core::IProtocolData
		{
			UNCOPYABLE(ArchProtocolData);
			UNMOVABLE(ArchProtocolData);
			friend ArchProtocol;
		public:
			ArchProtocolData();
			~ArchProtocolData();

		public:
			virtual service_id_t service_id() const noexcept override;
			virtual service_inst_id_t service_inst_id() const noexcept override;
			virtual void set_service_inst_id(service_inst_id_t id) noexcept override;
			virtual length_t length() const noexcept override;
			virtual const uint8_t* data() const noexcept override;

		private:
			std::vector<uint8_t>	_data;
			uint32_t				_version;
			std::array<uint8_t, 4>	_header_ext_cache;
			uint32_t				_header_ext_cache_idx;
			ArchParsingPhase		_parsing_phase;
			uint32_t				_content_length;
			service_id_t			_svc_id;
			service_inst_id_t		_svc_inst_id;
		};

	}

}






