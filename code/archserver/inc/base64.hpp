#pragma once

#include "archserver-prereq.hpp"

namespace arch
{

	class Base64Util
	{
	public:
		static inline bool is_base64(unsigned char c) {
			return (isalnum(c) || (c == '+') || (c == '/'));
		}

		static std::string encode(unsigned char const* bytes_to_encode, unsigned int in_len);
		static std::string decode(std::string const& encoded_string);

	private:
		static const std::string base64_chars;
	};

}

