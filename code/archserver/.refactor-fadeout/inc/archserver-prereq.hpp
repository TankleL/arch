#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <array>
#include <queue>
#include <stack>
#include <string>
#include <sstream>
#include <unordered_map>
#include <map>
#include <functional>
#include <algorithm>
#include <iterator>
#include <thread>
#include <mutex>
#include <cstdint>
#include <iomanip>
#include <cassert>
#include <cstring>
#include <exception>
#include <list>
#include <chrono>
#include <regex>

#ifdef WIN32
#	ifdef API_DEV_MOD
#		define	ARCH_API	__declspec(dllexport)
#	else
#		define	ARCH_API	__declspec(dllimport)
#	endif
#else
#	define ARCH_API
#endif

#define BYTES_SIZE_1K	1024
#define BYTES_SIZE_32K	32768
#define BYTES_SIZE_64K	65536
#define BYTES_SIZE_1M	1048576
#define BYTES_SIZE_1G	1073741824

namespace arch
{

	typedef unsigned char		byte_t, * byte_ptr;
	typedef const unsigned char	cbyte_t, * cbyte_ptr;
	typedef unsigned int		index_t, * index_ptr;

	template<typename _T>
	inline void safe_delete(_T*& target)
	{
		if (target)
		{
			delete target;
			target = nullptr;
		}
	}

	template<typename _T>
	inline void safe_delete_array(_T*& target)
	{
		if (target)
		{
			delete[] target;
			target = nullptr;
		}
	}

}



