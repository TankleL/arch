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
#include <filesystem>
#include <regex>

typedef size_t index_t;

#define UNMOVABLE(classname)	\
			classname(classname&&) = delete;	\
			classname& operator=(classname&&) = delete

#define UNCOPYABLE(classname)	\
		private:	\
			classname(const classname&) = delete;	\
			classname& operator=(const classname&) = delete

#define STATIC_CLASS(classname)	\
		private:	\
			classname(){}	\
			UNCOPYABLE(classname);	\
			UNMOVABLE(classname)



