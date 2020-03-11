#pragma once

#include <sstream>
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>


#include "getopt.h"
#include "asvc-service-instance.hpp"
#include "asvc-pipeserver.hpp"


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






