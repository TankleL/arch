#pragma once

#include "archserver-prereq.hpp"
#include "dllutils.hpp"
#include "protocols.hpp"

namespace arch
{

	class Module
	{
	public:
		Module(
			const std::string& pool_path,
			const std::string& name,
			const std::string& binname,
			ProtocolType protocol);

		bool	init();
		void	uninit();

	private:
		typedef int		(*module_init_fp_t)(void);
		typedef void	(*module_uninit_fp_t)(void);

	private:
		std::string				_name;
		std::string				_binname;
		std::string				_fullpath;
		ProtocolType			_protocol;
		
		osys::dll_handler_t		_hmodule;
		module_init_fp_t		_i_module_init;
		module_uninit_fp_t		_i_module_uninit;
	};

	class ModuleManager
	{
	public:
		void	load_all_modules();
		void	unload_all_modules();

	private:
		ProtocolType _get_protocol_type(const std::string& proto_type);

	private:
		std::unordered_map<ProtocolType, Module*>	_modules;
	};

}

