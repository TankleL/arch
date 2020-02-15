#include <cassert>
#include "message.hpp"

using namespace std;

Message::Message()
	: _id(0)
	, _data(nullptr)
	, _len(0)
{}

Message::Message(uint16_t length)
	: _id(0)
	, _len(length)
{
	_data = new uint8_t[length];
	memset(_data, 0, length);
}

Message::Message(uint32_t id, uint8_t* data, uint16_t length)
	: _id(id)
	, _data(data)
	, _len(length)
{}

Message::Message(const Message& rhs)
	: _id(rhs._id)
	, _data(nullptr)
	, _len(rhs._len)
{
	if (_len > 0)
	{
		assert(rhs._data);
		_data = new uint8_t[_len];
		memcpy(_data, rhs._data, _len);
	}
}

Message::Message(Message&& rhs) noexcept
	: _id(rhs._id)
	, _data(rhs._data)
	, _len(rhs._len)
{
	rhs.acquired();
}

Message::~Message()
{
	delete _data;
}

Message& Message::operator=(const Message& rhs)	// deep copy
{
	delete _data;
	_data = nullptr;

	_id = rhs._id;
	_len = rhs._len;
	if (_len > 0)
	{
		assert(rhs._data);
		_data = new uint8_t[_len];
		memcpy(_data, rhs._data, _len);
	}

	return *this;
}

Message& Message::operator=(Message&& rhs) noexcept
{
	_id = rhs._id;
	_data = rhs._data;
	_len = rhs._len;

	rhs.acquired();

	return *this;
}

void Message::acquire(Message& src)
{
	_id = src._id;
	_data = src._data;
	_len = src._len;

	src.acquired();
}

void Message::acquired()
{
	_id = 0;
	_data = nullptr;
	_len = 0;
}

uint32_t Message::get_id() const
{
	return _id;
}

void Message::set_id(uint32_t id)
{
	_id = id;
}

const uint8_t* Message::get_data() const
{
	return _data;
}

void Message::set_data(const std::vector<uint8_t>& data, uint16_t offset)
{
	assert((uint16_t)data.size() - offset <= _len);
	memcpy(_data, data.data() + offset, _len);
}

uint16_t Message::get_length() const
{
	return _len;
}


