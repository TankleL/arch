#pragma once

#include "archserver-prereq.hpp"

namespace arch
{
	namespace config
	{
		typedef struct server_phase_st
		{
			server_phase_st()
				: port(0)
				, backlog(128)
				, core_num(4)
				, max_living_conn(BYTES_SIZE_1M)
				, max_archmsg_queue_len(BYTES_SIZE_32K)
				, protocol(0)	// http
			{}

			std::string		ipaddr;
			int				port;
			int				backlog;
			int				core_num;
			int				max_living_conn;
			int				max_archmsg_queue_len;
			int				protocol;
		} server_phase_t;

		typedef struct module_st
		{
			std::string		name;
			std::string		bin_name;
			int				proto_type;
		} module_t;

		typedef struct module_phase_st
		{
			std::string		root_path;
			std::unordered_map<int, module_t>	modules;	// mapping protocol to module
		} module_phase_t;

		extern std::unordered_map<std::string, std::string>		global_keyvalues;
		extern server_phase_t									server_phase;
		extern module_phase_t									module_phase;
		
		bool load_config(const std::string& configfile);

		int protocol_name_to_val(const std::string& pname);
	}
}

