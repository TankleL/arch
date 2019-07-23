#include "protocol-websocket.hpp"

using namespace std;
using namespace arch;

ProtoProcRet ProtoProcWebSocket::proc_istrm(IProtocolObject& dest, const uv_buf_t* uvbuffer, ssize_t uvreadlen)
{
	ProtoProcRet retval = PPR_AGAIN;

	return retval;
}

bool ProtoProcWebSocket::proc_ostrm(std::string& obuffer, const IProtocolObject& src)
{
	bool retval = true;

	return retval;
}

bool ProtoProcWebSocket::proc_check_switch(ProtocolType& dest_proto, const IProtocolObject& obj)
{
	return false;
}


