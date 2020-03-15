#include "message.hpp"
#include "vuint.hpp"

Message::Message()
	: _id(0)
	, _payload(nullptr)
	, _payload_len(0)
{}

Message::Message(std::unique_ptr<archproto::IProtocolData>&& data)
	: Message()
{
	if (!from(std::move(data)))
	{
		throw std::runtime_error("bad protocol data.");
	}
}

bool
Message::from(std::unique_ptr<archproto::IProtocolData>&& data)
{
	_rawdata = std::move(data);

	const uint8_t* buf = _rawdata->data();
	const uint16_t len = _rawdata->length();

	VUInt vuinthelper;

	// decode msgid
	while (true)
	{
		const VUInt::DigestStatus digst = vuinthelper.digest(*buf);
		if (VUInt::DS_Idle == digst)
		{
			_id = vuinthelper.value();
			break;
		}
		else if (VUInt::DS_Bad == digst)
		{
			return false;
		}
		++buf;
	}

	// decode payload
	_payload = buf;
	_payload_len = len - vuinthelper.encoded_size();

	return true;
}

Message::id_t Message::id() const
{
	return _id;
}





