#include "message-mgr.hpp"
#include "msg-registry.hpp"
#include "msg-joinchnl.hpp"


void MsgRegistry::register_messages()
{
	MessageMgr::subscribe(MSG(JoinChannel), &SMsgJoinChannelAction::action);
}

void MsgRegistry::unregister_messages()
{
	throw "no implementation";
}








