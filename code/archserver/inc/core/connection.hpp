#pragma once

#include "pre-req.hpp"

typedef uint16_t	conn_id_t;
static const uint16_t max_conn_id = (uint16_t)-1;

template<class StreamT>
class Connection
{
public:
	Connection() noexcept
		: _id(0)
		, _closing(false)
	{}

	Connection(conn_id_t id) noexcept
		: _id(id)
		, _closing(false)
	{}

	void set_id(conn_id_t id) noexcept
	{
		_id = id;
	}

	conn_id_t get_id() const noexcept
	{
		return _id;
	}

	void set_stream(const std::shared_ptr<StreamT>& stream) noexcept
	{
		_stream = stream;
	}

	std::weak_ptr<StreamT> get_stream() const noexcept
	{
		return _stream;
	}

	void set_closing(bool closing) noexcept
	{
		_closing = closing;
	}

	bool get_closing() const noexcept
	{
		return _closing;
	}

private:
	std::shared_ptr<StreamT> _stream;
	conn_id_t	_id;
	bool		_closing;
};




