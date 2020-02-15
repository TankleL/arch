#pragma once
#include <cstdint>
#include <vector>

class Message
{
public:
	Message();
	Message(uint16_t length);
	Message(uint32_t id, uint8_t* data, uint16_t length);
	Message(const Message& rhs);	// deep copy
	Message(Message&& rhs) noexcept;
	virtual ~Message();

public:
	Message& operator=(const Message& rhs);	// deep copy
	Message& operator=(Message&& rhs) noexcept;

public:
	void acquire(Message& src);
	void acquired();

public:
	uint32_t get_id() const;
	const uint8_t* get_data() const;
	uint16_t get_length() const;

	void set_id(uint32_t id);
	void set_data(const std::vector<uint8_t>& data, uint16_t offset = 0);

private:
	uint32_t	_id;
	uint8_t*	_data;
	uint16_t	_len;
};

namespace MessageUtil
{
	/**
	 * @func make_message_id
	 * @brief
	 *  /------------------------------------\
	 * ||   0 ~ 23   | 24 ~ 27 |   28 ~ 31   ||
	 * ||------------------------------------||
	 * ||  event_id  |   ext   | server_type ||
	 *  \------------------------------------/
	 */
	inline uint32_t make_id(
		int server_type,	/* value range: [0x0, 0xf] */
		int event_id,		/* value range: [0x000000, 0xffffff] */
		int ext = 0			/* value range: [0x0, 0xf] */)
	{
		return  (server_type << 28) |
			(ext << 24) |
			(event_id);
	}

	inline int get_servertype(uint32_t message_id)
	{
		return message_id >> 28;
	}

	inline int get_event_id(uint32_t message_id)
	{
		return (message_id & 0x00ffffff);
	}
}

