// CCF - Connection Control Flags

#pragma once
#include <cstdint>

namespace archsvc
{

	enum ConnectionControlFlag : uint16_t
	{
		CCF_Close = 0x01,
	};

    inline bool ccf_check(uint16_t combined, ConnectionControlFlag flag)
    {
		return (combined & flag) == 1;
    }

	inline uint16_t ccf_set_true(uint16_t origin, ConnectionControlFlag flag)
	{
		return (origin | flag);
	}

	inline uint16_t ccf_set_false(uint16_t origin, ConnectionControlFlag flag)
	{
		return (origin & (~flag));
	}

}




