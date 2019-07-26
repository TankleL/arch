#pragma once

#include "disposable.hpp"
#include "acquirable.hpp"

namespace arch
{

	class IServiceDataObject
		: public IDisposable
		, public IAcquriable<IServiceDataObject>
	{};

}

