#pragma once

#include "archserver-prereq.hpp"
#include "module.hpp"

namespace arch
{
	class ModuleManager
	{
	public:
		ModuleManager();
		virtual	~ModuleManager();
	public:
		void	load_all_modules();
		void	unload_all_modules();

	public:
		IModule* get_module(int proto_number) const;

	private:
		void	_load_builtin_modules();
		void	_load_plugin_modules();

	private:
		int _get_protocol_type(const std::string& proto_type);

	private:
		std::unordered_map<int, IModule*>	_modules;
	};
}

