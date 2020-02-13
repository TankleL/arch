#pragma once

#include "archserver-prereq.hpp"

namespace arch
{
	class ArchException
	{
	public:
		explicit ArchException(const std::string& msg, uint32_t error = 0) noexcept
			: _msg(msg)
			, _error(error)
		{}

		virtual const char* const message() const noexcept
		{
			return _msg.c_str();
		}

		virtual uint32_t error() const noexcept
		{
			return _error;
		}

	protected:
		std::string	_msg;
		uint32_t	_error;
	};

	class ArchException_ItemAlreadyExist : public ArchException
	{
	public:
		ArchException_ItemAlreadyExist() noexcept
			: ArchException("The item has already existed")
		{}
	};

	class ArchException_ItemAllocFailed : public ArchException
	{
	public:
		ArchException_ItemAllocFailed() noexcept
			: ArchException("Failed to allocate new item")
		{}
	};

	class ArchException_IndexOutOfBoundary : public ArchException
	{
	public:
		ArchException_IndexOutOfBoundary() noexcept
			: ArchException("index is out of boundary")
		{}
	};

	class ArchException_ReBind : public ArchException
	{
	public:
		ArchException_ReBind() noexcept
			: ArchException("already bound a object")
		{}
	};

	class ArchException_NoImplementation : public ArchException
	{
	public:
		ArchException_NoImplementation() noexcept
			: ArchException("no implementation")
		{}
	};
}
