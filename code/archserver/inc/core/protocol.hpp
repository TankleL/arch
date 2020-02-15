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
		virtual ProtoProcRet	proc_istrm(std::any& dest, std::uint8_t* readbuf, size_t toreadlen, size_t& procbytes) = 0;
		virtual bool			proc_ostrm(std::string& obuffer, const std::any& src) = 0;
		virtual bool			proc_check_switch(ProtocolType& dest_proto, const std::any& obj) = 0;
	};

}









