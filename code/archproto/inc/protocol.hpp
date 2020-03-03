#pragma once

#include <cstdint>
#include <vector>

namespace archproto
{
	/******************************************************************/
	/*                    ------ Definition ------                    */
	/*                          ProtocolType                          */
	/******************************************************************/
	enum ProtocolType : uint16_t
	{
		PT_Http = 0,
		PT_WebSocket = 1,
		PT_Arch,

		// < ----------- insert new types here.

		PT_ProtoTypesNum,
		PT_Unknown = (uint16_t)-1
	};


	/******************************************************************/
	/*                   ------ Declaration ------                    */
	/*                         IProtocolData                          */
	/******************************************************************/
	class IProtocolData
	{
	public:
		typedef uint32_t service_id_t;
		typedef uint16_t service_inst_id_t;
		typedef uint16_t length_t;
		
	public:
		virtual service_id_t		service_id() const noexcept = 0;
		virtual service_inst_id_t	service_inst_id() const noexcept = 0;
		virtual void				set_service_id(service_id_t id) noexcept = 0;
		virtual void				set_service_inst_id(service_inst_id_t id) noexcept = 0;
		virtual length_t			length() const noexcept = 0;
		virtual const std::uint8_t*	data() const noexcept = 0;
	};


	/******************************************************************/
	/*                   ------ Declaration ------                    */
	/*                        IProtocolHandler                        */
	/******************************************************************/
	class IProtocolHandler
	{
	public:
		enum ProtoProcRet : int
		{
			PPR_ERROR	= -2,
			PPR_CLOSE	= -1,

			PPR_AGAIN	= 0,
			PPR_PULSE,
		};

	public:
		virtual ProtocolType	get_protocol_type() const noexcept = 0;
		virtual ProtoProcRet	des_sock_stream(IProtocolData& dest, const uint8_t* readbuf, size_t toreadlen, size_t& procbytes) = 0;
		virtual bool			ser_sock_stream(std::vector<uint8_t>& dest, const IProtocolData& obj) = 0;
		virtual bool			chk_sock_protoswitch(ProtocolType& dest_proto, const IProtocolData& obj) = 0;
		virtual ProtoProcRet	des_service_stream(IProtocolData& dest, const uint8_t* src, size_t toreadlen, size_t& procbytes) = 0;
		virtual bool			ser_service_stream(std::vector<uint8_t>& dest, const IProtocolData& obj) = 0;
	};

}


