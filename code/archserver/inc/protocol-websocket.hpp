#pragma once

#include "archserver-prereq.hpp"
#include "protocols.hpp"
#include "protocolproc.hpp"

namespace arch
{

	enum WSParsingPhase : int
	{
		WSPP_Start = 0,
		WSPP_Again,

		WSPP_FinRsvsOpcode,
		WSPP_MaskPayloadLen,
		WSPP_ExtPayloadLen16,
		WSPP_ExtPayloadLen64,
		WSPP_MaskingKey,
		WSPP_Payload,

		WSPP_Ping,
		WSPP_Pong,
		WSPP_End,
		WSPP_Error
	};

	class Internal_ProtoObjectWebSocket;
	class ProtoProcWebSocket : public IProtocolProc
	{
	public:
		virtual ProtoProcRet proc_istrm(IProtocolObject& dest, const uv_buf_t* uvbuffer, ssize_t uvreadlen) override;
		virtual bool proc_ostrm(std::string& obuffer, const IProtocolObject& src) override;
		virtual bool proc_check_switch(ProtocolType& dest_proto, const IProtocolObject& obj) override;

	protected:
		static WSParsingPhase _proc_istrm_byte(Internal_ProtoObjectWebSocket& obj, byte_ptr read_buf, byte_ptr p, index_t idx);
		
	};
		
	class Internal_ProtoObjectWebSocket
		: public ProtocolObjectWebSocket
	{
		friend ProtoProcWebSocket;

	private:
		typedef std::vector<byte_t>				buffer_t;

	private:
		buffer_t				_cache;

		index_t					_commit_pos;
		WSParsingPhase			_parsing_phase;
	};

}


