#pragma once

#include "pre-req.hpp"


class MsgRegistry
{
	STATIC_CLASS(MsgRegistry);

public:
	static void register_messages();
	static void unregister_messages();
};





