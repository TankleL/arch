#include "buffer.hpp"
#include "ex-general.hpp"

using namespace std;
using namespace core;

/************************************************************************/
/*                             Buffer                               */
/************************************************************************/
Buffer::Buffer() noexcept
	: _buf(nullptr)
	, _len(0)
{}

Buffer::Buffer(Buffer&& rhs) noexcept
	: _buf(rhs._buf)
	, _len(rhs._len)
{
	rhs.acquired();
}

Buffer::Buffer(const Buffer& rhs)
	: _buf(nullptr)
	, _len(rhs._len)
{
	if (_len)
	{
		try
		{
			_buf = new uint8_t[_len];
		}
		catch (const std::exception & e)
		{
			_buf = nullptr;
			_len = 0;
			throw e;
		}
		
		memcpy(_buf, rhs._buf, _len);
	}
}

Buffer::Buffer(index_t length)
	: _buf(nullptr)
	, _len(length)
{
	if (_len)
	{
		try
		{
			_buf = new uint8_t[_len];
		}
		catch (const std::exception & e)
		{
			_buf = nullptr;
			_len = 0;
			throw e;
		}
		_buf[_len - 1] = 0;
	}
}

Buffer::~Buffer() noexcept
{
	_free_buffer();
}

Buffer& Buffer::operator=(Buffer&& rhs) noexcept
{
	_buf = rhs._buf;
	_len = rhs._len;

	rhs.acquired();

	return *this;
}

Buffer& Buffer::operator=(const Buffer& rhs)
{
	_buf = nullptr;
	_len = rhs._len;

	if (_len)
	{
		_buf = new uint8_t[_len];
		memcpy(_buf, rhs._buf, _len);
	}

	return *this;
}

uint8_t* Buffer::buffer(index_t offset /* = 0 */) const noexcept
{
	return _buf + offset;
}

index_t Buffer::length() const noexcept
{
	return _len;
}

Buffer& Buffer::acquire(Buffer& src) noexcept
{
	_buf = src._buf;
	_len = src._len;

	src.acquired();

	return *this;
}

void Buffer::acquired() noexcept
{
	_buf = nullptr;
	_len = 0;
}

void Buffer::clear() noexcept
{
	_free_buffer();
}

void Buffer::resize(index_t length)
{
	_free_buffer();
	if (length > 0)
	{
		try
		{
			_buf = new uint8_t[length];
		}
		catch (const std::exception & e)
		{
			_buf = nullptr;
			_len = 0;
			throw e;
		}
		
		_len = length;
	}
}

void Buffer::_free_buffer() noexcept
{
	if (_buf)
	{
		delete[] _buf;
		_buf = nullptr;
		_len = 0;
	}
}

uint8_t Buffer::at(index_t index) const
{
#ifdef _DEBUG
	if (index >= _len)
		throw ArchException_IndexOutOfBoundary();
#endif

	return _buf[index];
}

uint8_t& Buffer::operator[](index_t index)
{
#ifdef _DEBUG
	if (index >= _len)
		throw ArchException_IndexOutOfBoundary();
#endif

	return _buf[index];
}

void Buffer::move_inplace(index_t dest, index_t src, index_t len)
{
#ifdef _DEBUG
	index_t right = dest >= src ? dest : src;
	if (right + len >= _len)
		throw ArchException_IndexOutOfBoundary();
#endif

	if (dest < src)
	{
		for (index_t i = 0; i < len; ++i)
		{
			_buf[dest + i] = _buf[src + i];
		}
	}
	else if(dest > src)
	{
		for (index_t i = 1; i <= len; ++i)
		{
			_buf[dest + len - i] = _buf[src + len - i];
		}
	}	
}
