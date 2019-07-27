#pragma once

#include "archserver-prereq.hpp"

namespace arch
{
	namespace config
	{

		typedef struct server_phase_st
		{
			int		port;
			int		backlog;
			int		max_living_conn;
			int		max_archmsg_queue_len;
		} server_phase_t;

		typedef struct module_st
		{
			std::string		name;
			std::string		bin_name;
			std::string		proto_type;
		} module_t;

		typedef struct module_phase_st
		{
			std::string		root_path;
			std::unordered_map<std::string, module_t>	modules;
		} module_phase_t;

		extern std::unordered_map<std::string, std::string>		global_keyvalues;
		extern server_phase_t									server_phase;
		extern module_phase_t									module_phase;
		
		bool load_config(const std::string& configfile);

	}
}

