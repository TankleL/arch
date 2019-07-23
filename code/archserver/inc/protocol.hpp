#pragma once

#include "archserver-prereq.hpp"
#include "uv.h"
#include "tcpservice.hpp"

namespace arch
{

	enum ProtoProcRet : int
	{
		PPR_ERROR	= -2,
		PPR_CLOSE	= -1,

		PPR_AGAIN	= 0,
		PPR_PULSE,
	};

	class IProtocolProc
	{
	public:
		virtual ProtoProcRet proc_istrm(IProtocolObject& dest, const uv_buf_t* uvbuffer, ssize_t uvreadlen) = 0;
		virtual bool proc_ostrm(std::string& obuffer, const IProtocolObject& src) = 0;
		virtual bool proc_check_switch(ProtocolType& dest_proto, const IProtocolObject& obj) = 0;
	};

}

