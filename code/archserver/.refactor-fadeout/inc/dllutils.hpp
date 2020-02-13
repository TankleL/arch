#pragma once

#include "archserver-prereq.hpp"

namespace arch
{
	namespace osys
	{
		typedef void* dll_handler_t;
		typedef void* dll_raw_symbol_t;

		extern dll_handler_t		dll_invalid_handler;

		dll_handler_t		dll_load(const std::string& filepath);
		dll_raw_symbol_t	dll_symbol(dll_handler_t handle, const char* name);
		void				dll_unload(dll_handler_t handle);
	}
}

