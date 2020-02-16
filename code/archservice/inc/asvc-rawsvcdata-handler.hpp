#pragma once

#include <cstdint>

namespace archsvc
{

	class RawSvcDataHandler
	{
	public:
		void deserialize(const std::uint8_t* data, size_t length);
		void serialiaze();
	};

}


