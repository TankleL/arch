#pragma once

#include "archserver-prereq.hpp"
#include "protocols.hpp"
#include "protocolproc.hpp"

namespace arch
{

	enum WSParsingPhase : int
	{
		WSPPStart = 0,
		WSPPAgain,
		WSPPEnd,


		WSPPError
	};

	class Internal_ProtoObjectWebSocket;
	class ProtoProcWebSocket : public IProtocolProc
	{
	public:
		virtual ProtoProcRet proc_istrm(IProtocolObject& dest, const uv_buf_t* uvbuffer, ssize_t uvreadlen) override;
		virtual bool proc_ostrm(std::string& obuffer, const IProtocolObject& src) override;
		virtual bool proc_check_switch(ProtocolType& dest_proto, const IProtocolObject& obj) override;
	};
		
	class Internal_ProtoObjectWebSocket
		: public ProtocolObjectWebSocket
	{
		friend ProtoProcWebSocket;
	};

}


