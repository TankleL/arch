#pragma once

#include "archserver-prereq.hpp"
#include "acquirable.hpp"
#include "disposable.hpp"

namespace arch
{
	/**
	* @module ArchBuffer
	*/
	class ArchBuffer : 
		public IAcquriable<ArchBuffer>,
		public IDisposable
	{
	public:
		ArchBuffer() noexcept;
		ArchBuffer(index_t length);
		ArchBuffer(ArchBuffer&& rhs) noexcept;
		ArchBuffer(const ArchBuffer& rhs);
		~ArchBuffer() noexcept;

		ArchBuffer& operator=(ArchBuffer&& rhs) noexcept;
		ArchBuffer& operator=(const ArchBuffer& rhs);

	public:
		virtual ArchBuffer& acquire(ArchBuffer& src) noexcept override;
		virtual void dispose() noexcept override;

	public:
		void clear() noexcept;
		void resize(index_t length);
		byte_t* buffer(index_t offset = 0) const noexcept;
		index_t length() const noexcept;
		byte_t at(index_t index) const;
		void move_inplace(index_t dest, index_t src, index_t len);

	public:
		byte_t& operator[](index_t index);

	protected:
		void _free_buffer() noexcept;

	protected:
		byte_t*	_buf;
		index_t	_len;
	};

	static ArchBuffer	EmptyBuffer;
}