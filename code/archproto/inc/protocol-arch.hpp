#pragma once
#include "protocol.hpp"
#include "vuint.hpp"

namespace archproto
{


	/******************************************************************/
	/*                    ARCH PROTO MESSAGE PACK                     */
	/*                                                                */
	/*     # V0.1                                                     */
	/*         - [HEADER] Version      -> VUInt                       */
	/*         - [HEADER] svc_id       -> VUInt                       */
	/*         - [HEADER] svc_inst_id  -> VUInt                       */
	/*         - [HEADER] length       -> VUInt 			          */
	/*         - [CONTENT] Data        -> length byte(s)              */
	/*                                                                */
	/******************************************************************/

	enum ArchProtocolVersion : uint32_t
	{
		APV_Unknown,
		APV_0_1		// arch protocol 0.1
	};

	enum ArchParsingPhase : int
	{
		APP_Start = 0,
		APP_Parsing_Header_Version,
		APP_Parsing_Header_SVC_ID,
		APP_Parsing_Header_SVC_INST_ID,
		APP_Parsing_Header_Length,
		APP_Parsing_Content
	};

	class ArchProtocol : public IProtocolHandler
	{
	public:
		virtual ProtocolType	get_protocol_type() const noexcept override;
		virtual ProtoProcRet	des_sock_stream(IProtocolData& dest, const uint8_t* readbuf, size_t toreadlen, size_t& procbytes) override;
		virtual bool			ser_sock_stream(std::vector<uint8_t>& dest, const IProtocolData& obj) override;
		virtual bool			chk_sock_protoswitch(ProtocolType& dest_proto, const IProtocolData& obj) override;
		virtual ProtoProcRet	des_service_stream(IProtocolData& dest, const uint8_t* src, size_t toreadlen, size_t& procbytes) override;
		virtual bool			ser_service_stream(std::vector<uint8_t>& dest, const IProtocolData& obj) override;
	};

	class ArchProtocolData : public IProtocolData
	{
		ArchProtocolData(const ArchProtocolData&) = delete;
		ArchProtocolData& operator=(const ArchProtocolData&) = delete;

		friend ArchProtocol;

	public:
		ArchProtocolData();
		~ArchProtocolData();

	public:
		virtual service_id_t		service_id() const noexcept override;
		virtual service_inst_id_t	service_inst_id() const noexcept override;
		virtual void				set_service_id(service_id_t id) noexcept override;
		virtual void				set_service_inst_id(service_inst_id_t id) noexcept override;
		virtual length_t			length() const noexcept override;
		virtual const uint8_t*		data() const noexcept override;

	private:
		std::vector<uint8_t>	_data;
		VUInt					_version;
		VUInt					_svc_id;
		VUInt					_svc_inst_id;
		VUInt					_content_length;

		ArchParsingPhase		_parsing_phase;
	};


}









