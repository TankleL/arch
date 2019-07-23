#pragma once

#include "archserver-prereq.hpp"

namespace arch
{
	template<typename _T>
	class IAcquriable
	{
	public:
		/**
		* once this interface has been called, the src object should lost its control of memory.
		*/
		virtual _T& acquire(_T& src) noexcept = 0;
	};
}
