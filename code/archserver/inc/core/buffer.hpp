#pragma once

#include "pre-req.hpp"
#include "acquirable.hpp"

namespace core
{

	class Buffer : public IAcquriable<Buffer>
	{
	public:
		Buffer() noexcept;
		Buffer(index_t length);
		Buffer(Buffer&& rhs) noexcept;
		Buffer(const Buffer& rhs);
		~Buffer() noexcept;

		Buffer& operator=(Buffer&& rhs) noexcept;
		Buffer& operator=(const Buffer& rhs);

	public:
		virtual Buffer& acquire(Buffer& src) noexcept override;
		virtual void acquired() noexcept override;

	public:
		void clear() noexcept;
		void resize(index_t length);
		uint8_t* buffer(index_t offset = 0) const noexcept;
		index_t length() const noexcept;
		uint8_t at(index_t index) const;
		void move_inplace(index_t dest, index_t src, index_t len);

	public:
		uint8_t& operator[](index_t index);

	protected:
		void _free_buffer() noexcept;

	protected:
		uint8_t* _buf;
		index_t	_len;
	};

	static Buffer	EmptyBuffer;

}

