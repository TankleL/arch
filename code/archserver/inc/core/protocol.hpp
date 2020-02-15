#pragma once

#include "pre-req.hpp"
#include "acquirable.hpp"

namespace core
{

	/******************************************************************/
	/*                    ------ Definition ------                    */
	/*                          ProtocolType                          */
	/******************************************************************/
	enum ProtocolType : int
	{
		PT_Http = 0,
		PT_WebSocket = 1,
		PT_Arch,

		// < ----------- insert new types here.

		PT_ProtoTypesNum,
		PT_Unknown = -1
	};


	/******************************************************************/
	/*                   ------ Declaration ------                    */
	/*                         IProtocolData                          */
	/******************************************************************/
	class IProtocolData
	{
	public:
		typedef uint32_t service_id_t;
		
	public:
		virtual service_id_t service_id() const noexcept = 0;
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
		virtual ProtoProcRet	proc_istrm(IProtocolData& dest, std::uint8_t* readbuf, size_t toreadlen, size_t& procbytes) = 0;
		virtual bool			proc_ostrm(std::string& obuffer, const IProtocolData& src) = 0;
		virtual bool			proc_check_switch(ProtocolType& dest_proto, const IProtocolData& obj) = 0;
	};

}









