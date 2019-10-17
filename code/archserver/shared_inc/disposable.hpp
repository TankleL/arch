#pragma once


namespace arch
{
	/*
	* This is a temporary design. This must be replaced or redesign in future.
	*/
	class IDisposable
	{
	public:
		/**
		* once this interface has been called, the object should not be released from memory again.
		*/
		virtual void dispose() noexcept = 0;
	};
}

