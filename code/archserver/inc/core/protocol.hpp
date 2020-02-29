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
		typedef uint16_t service_inst_id_t;
		typedef uint16_t length_t;
		
	public:
		virtual service_id_t		service_id() const noexcept = 0;
		virtual service_inst_id_t	service_inst_id() const noexcept = 0;
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
		virtual ProtoProcRet	proc_istrm(IProtocolData& dest, std::uint8_t* readbuf, size_t toreadlen, size_t& procbytes) = 0;
		virtual bool			proc_ostrm(std::vector<uint8_t>& obuffer, const IProtocolData& rsp, const IProtocolData& req) = 0;
		virtual bool			proc_check_switch(ProtocolType& dest_proto, const IProtocolData& obj) = 0;
	};




	/******************************************************************/
	/*                  ------ Decl. & Impl.------                    */
	/*                        PlainProtocolData                       */
	/******************************************************************/

	class PlainProtocolData : public IProtocolData
	{
	public:
		PlainProtocolData();
		~PlainProtocolData();

	public:
		virtual service_id_t		service_id() const noexcept;
		virtual service_inst_id_t	service_inst_id() const noexcept;
		virtual void				set_service_inst_id(service_inst_id_t id) noexcept;
		virtual length_t			length() const noexcept;
		virtual const std::uint8_t* data() const noexcept;

	public:
		service_id_t			svc_id;
		service_inst_id_t		svc_inst_id;
		std::vector<uint8_t>	protodata;
	};

	inline PlainProtocolData::PlainProtocolData()
		: svc_id(0)
		, svc_inst_id(0)
	{}

	inline PlainProtocolData::~PlainProtocolData()
	{}

	inline PlainProtocolData::service_id_t PlainProtocolData::service_id() const noexcept
	{
		return svc_id;
	}

	inline PlainProtocolData::service_inst_id_t PlainProtocolData::service_inst_id() const noexcept
	{
		return svc_inst_id;
	}

	inline void PlainProtocolData::set_service_inst_id(service_inst_id_t id) noexcept
	{
		svc_inst_id = id;
	}

	inline PlainProtocolData::length_t PlainProtocolData::length() const noexcept
	{
		return (length_t)protodata.size();
	}

	inline const std::uint8_t* PlainProtocolData::data() const noexcept
	{
		return protodata.data();
	}
	
}









