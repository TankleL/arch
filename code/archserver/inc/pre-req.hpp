#pragma once

#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <exception>
#include <cstdint>
#include <cmath>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <limits>
#include <any>
#include <array>
#include <cassert>
#include <deque>

typedef size_t index_t;

#define UNCOPYABLE(name)	\
		private:	\
			name(const name&) = delete;	\
			name(name&&) = delete;	\
			name& operator=(const name&) = delete;	\
			name& operator=(name&&) = delete

#define STATIC_CLASS(classname)	\
		private:	\
			classname(){}	\
			UNCOPYABLE(classname)


