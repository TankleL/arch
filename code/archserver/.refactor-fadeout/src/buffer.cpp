
#include "buffer.hpp"
#include "archexception.hpp"

using namespace std;
using namespace arch;

/************************************************************************/
/*                             ArchBuffer                               */
/************************************************************************/
ArchBuffer::ArchBuffer() noexcept
	: _buf(nullptr)
	, _len(0)
{}

ArchBuffer::ArchBuffer(ArchBuffer&& rhs) noexcept
	: _buf(rhs._buf)
	, _len(rhs._len)
{
	rhs._buf = nullptr;
	rhs._len = 0;
}

ArchBuffer::ArchBuffer(const ArchBuffer& rhs)
	: _buf(nullptr)
	, _len(rhs._len)
{
	if (_len)
	{
		try
		{
			_buf = new byte_t[_len];
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

ArchBuffer::ArchBuffer(index_t length)
	: _buf(nullptr)
	, _len(length)
{
	if (_len)
	{
		try
		{
			_buf = new byte_t[_len];
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

ArchBuffer::~ArchBuffer() noexcept
{
	_free_buffer();
}

ArchBuffer& ArchBuffer::operator=(ArchBuffer&& rhs) noexcept
{
	_buf = rhs._buf;
	_len = rhs._len;
	rhs._buf = nullptr;
	rhs._len = 0;

	return *this;
}

ArchBuffer& ArchBuffer::operator=(const ArchBuffer& rhs)
{
	_buf = nullptr;
	_len = rhs._len;

	if (_len)
	{
		_buf = new byte_t[_len];
		memcpy(_buf, rhs._buf, _len);
	}

	return *this;
}

byte_t* ArchBuffer::buffer(index_t offset /* = 0 */) const noexcept
{
	return _buf + offset;
}

index_t ArchBuffer::length() const noexcept
{
	return _len;
}

ArchBuffer& ArchBuffer::acquire(ArchBuffer& src) noexcept
{
	_buf = src._buf;
	_len = src._len;
	src._buf = nullptr;
	src._len = 0;

	return *this;
}

void ArchBuffer::clear() noexcept
{
	_free_buffer();
}

void ArchBuffer::resize(index_t length)
{
	_free_buffer();
	if (length > 0)
	{
		try
		{
			_buf = new byte_t[length];
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

void ArchBuffer::_free_buffer() noexcept
{
	if (_buf)
	{
		delete[] _buf;
		_buf = nullptr;
		_len = 0;
	}
}

void ArchBuffer::dispose() noexcept
{
	delete this;
}

byte_t ArchBuffer::at(index_t index) const
{
#ifdef _DEBUG
	if (index >= _len)
		throw ArchException_IndexOutOfBoundary();
#endif

	return _buf[index];
}

byte_t& ArchBuffer::operator[](index_t index)
{
#ifdef _DEBUG
	if (index >= _len)
		throw ArchException_IndexOutOfBoundary();
#endif

	return _buf[index];
}

void ArchBuffer::move_inplace(index_t dest, index_t src, index_t len)
{
#ifdef _DEBUG
	index_t right = std::max(dest, src);
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

