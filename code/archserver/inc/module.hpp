#pragma once

#include "archserver-prereq.hpp"
#include "dllutils.hpp"
#include "protocols.hpp"
#include "archmessage.hpp"
#include "service-processor.hpp"

namespace arch
{

	class Module
		: public IServiceProcessor
	{
	public:
		Module(
			const std::string& pool_path,
			const std::string& name,
			const std::string& binname,
			ProtocolType protocol);

		bool	init();
		void	uninit();

	public:
		virtual void process(ArchMessage& onode, const ArchMessage& inode) override;

	private:
		typedef int		(*module_init_fp_t)(void);
		typedef void	(*module_uninit_fp_t)(void);
		typedef void	(*module_service_proc_fp_t)(ArchMessage&, const ArchMessage&);

	private:
		std::string				_name;
		std::string				_binname;
		std::string				_fullpath;
		ProtocolType			_protocol;
		
		osys::dll_handler_t			_hmodule;
		module_init_fp_t			_i_module_init;
		module_uninit_fp_t			_i_module_uninit;
		module_service_proc_fp_t	_i_module_service_proc;
	};

	class ModuleManager
	{
	public:
		ModuleManager();
		virtual	~ModuleManager();
	public:
		void	load_all_modules();
		void	unload_all_modules();

	public:
		Module& get_module(ProtocolType proto_type) const;

	private:
		ProtocolType _get_protocol_type(const std::string& proto_type);

	private:
		Module*		_modules[PT_ProtoTypesNum];
	};

}

