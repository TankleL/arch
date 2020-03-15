#pragma once

#include "pre-req.hpp"
#include "service-msg.hpp"
#include "message-mgr.hpp"

DEF_MSG(JoinChannel, 10000);

class SMsgJoinChannelReq : public ServiceMessage
{

};

class SMsgJoinChannelRsp : public ServiceMessage
{};

class SMsgJoinChannelAction
{
	STATIC_CLASS(SMsgJoinChannelAction);

public:
	static void action(const Message& msg);
};





