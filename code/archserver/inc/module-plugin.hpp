#pragma once

#include "archserver-prereq.hpp"
#include "module.hpp"

namespace arch
{

	class PluginModule
		: public IModule
	{
	public:
		PluginModule(
			const std::string& pool_path,
			const std::string& name,
			const std::string& binname,
			int protocol_number);

	public:
		virtual bool	init() override;
		virtual void	uninit() override;
		virtual void	process(ArchMessage& onode, const ArchMessage& inode) override;
		virtual void	dispose() noexcept override;

	private:
		typedef int		(*module_init_fp_t)(void);
		typedef void	(*module_uninit_fp_t)(void);
		typedef void	(*module_service_proc_fp_t)(ArchMessage&, const ArchMessage&);

	private:
		std::string				_name;
		std::string				_binname;
		std::string				_fullpath;
		int						_proto_num;

		osys::dll_handler_t			_hmodule;
		module_init_fp_t			_i_module_init;
		module_uninit_fp_t			_i_module_uninit;
		module_service_proc_fp_t	_i_module_service_proc;
	};

}


