#pragma once

#include "pre-req.hpp"

class Message
{
	UNCOPYABLE(Message);
public:
	typedef uint32_t	id_t;

public:
	Message();
	Message(std::unique_ptr<archproto::IProtocolData>&& data);

public:
	bool	from(std::unique_ptr<archproto::IProtocolData>&& data);
	id_t	id() const;

private:
	std::unique_ptr<archproto::IProtocolData> _rawdata;

	id_t				_id;
	const uint8_t*		_payload;
	uint32_t			_payload_len;
};



