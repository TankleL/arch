#include "message-mgr.hpp"

MessageMgr::msggroup_t MessageMgr::_msggroups;

size_t MessageMgr::subscribe(
	const Message::id_t& msgid,
	const subscriber_action_t& action)
{
	auto& sublist = _msggroups[msgid];

	size_t idx = ++sublist.topidx;
	sublist.subs.push_back(subscriber_t(action, idx));

	return idx;
}


void MessageMgr::unsubscribe(
	const Message::id_t& msgid,
	size_t index)
{
	auto& sublist = _msggroups[msgid];

	for(auto sub = sublist.subs.begin();
		sub != sublist.subs.end();
		++sub)
	{
		if (index == sub->index)
		{
			sublist.subs.erase(sub);
		}
	}
}

