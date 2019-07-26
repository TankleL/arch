#pragma once


namespace arch
{
	class IDisposable
	{
	public:
		/**
		* once this interface has been called, the object should not be released from memory again.
		*/
		virtual void dispose() noexcept = 0;
	};
}

