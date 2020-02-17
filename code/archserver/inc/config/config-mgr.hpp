#pragma once

#include "pre-req.hpp"

namespace config
{

	class ConfigMgr
	{
		STATIC_CLASS(ConfigMgr);
	public:
		static void load_configs(const std::string& configdir);

	private:
		static void _load_master_config(const std::string& filename);
		static void _load_master_cfg_listen(const void* xelement);
		static void _load_master_cfg_service(const void* xelement);
		static void _load_master_cfg_service_inst(void* insts, const void* xelement);
	};

}



