#pragma once

#include "pre-req.hpp"
#include "message.hpp"

class MessageMgr
{
	STATIC_CLASS(MessageMgr);

public:
	typedef std::function<void(void)> subscriber_action_t;

	typedef struct subscriber_s
	{
		subscriber_s(
			subscriber_action_t act,
			size_t idx)
			: action(act)
			, index(idx)
		{}

		subscriber_action_t	action;
		size_t				index;
	} subscriber_t;

	typedef struct subscriber_list_s
	{
		subscriber_list_s()
			: topidx(0)
		{}

		std::vector<subscriber_t>	subs;
		size_t						topidx;
	} subscriber_list_t;

	typedef std::unordered_map<
		Message::id_t,
		subscriber_list_t>	msggroup_t;

public:
	static size_t subscribe(
		const Message::id_t& msgid,
		const subscriber_action_t& subscriber);
	static void unsubscribe(
		const Message::id_t& msgid,
		size_t index);

private:
	static msggroup_t	_msggroups;
};




