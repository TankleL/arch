#pragma once

template<typename _T>
class IAcquriable
{
public:
	/**
	* once this interface has been called, the src object should lost its control of memory.
	*/
	virtual _T& acquire(_T& src) noexcept = 0;
	
	/**
	* indicates the object has been acquired.
	*/
	virtual void acquired() noexcept = 0;
};

