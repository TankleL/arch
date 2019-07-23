#pragma once

#include "archserver-prereq.hpp"

namespace arch
{
	namespace utils
	{

		const char* _c_au_whitespaces = " \t\n\r\f\v";

		// trim from end of string (right)
		inline std::string& rtrim(std::string& s)
		{
			s.erase(s.find_last_not_of(_c_au_whitespaces) + 1);
			return s;
		}

		// trim from beginning of string (left)
		inline std::string& ltrim(std::string & s)
		{
			s.erase(0, s.find_first_not_of(_c_au_whitespaces));
			return s;
		}

		// trim from both ends of string (right then left)
		inline std::string& trim(std::string & s)
		{
			return ltrim(rtrim(s));
		}

	}
}