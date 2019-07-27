#include "module.hpp"
#include "config.hpp"

using namespace std;
using namespace arch;

Module::Module(
	const std::string& pool_path,
	const std::string& name,
	const std::string& binname,
	ProtocolType protocol)
	: _name(name)
	, _binname(binname)
	, _fullpath(pool_path + "/" + name)
	, _protocol(protocol)
	, _hmodule(nullptr)
	, _i_module_init(nullptr)
	, _i_module_uninit(nullptr)
{}

bool Module::init()
{
	bool retval = true;

	_hmodule = osys::dll_load(_fullpath + "/" + _binname);
	if (_hmodule != osys::dll_invalid_handler)
	{
		_i_module_init = static_cast<module_init_fp_t>(
			osys::dll_symbol(_hmodule, "arch_module_init"));
		retval = retval && _i_module_init != nullptr;

		_i_module_uninit = static_cast<module_uninit_fp_t>(
			osys::dll_symbol(_hmodule, "arch_module_uninit"));
		retval = retval && _i_module_uninit != nullptr;

		if (!retval)
		{
			osys::dll_unload(_hmodule);
		}

		retval = retval && (0 == _i_module_init());
	}
	else
	{
		retval = false;
	}

	return retval;
}

void Module::uninit()
{
	_i_module_uninit();
	osys::dll_unload(_hmodule);
}


ProtocolType ModuleManager::_get_protocol_type(const std::string& proto_type)
{
	if (proto_type == "HTTP")
	{
		return PT_Http;
	}
	else
	{
		return PT_Unknown;
	}
}

void ModuleManager::load_all_modules()
{
	for (const auto& m : config::module_phase.modules)
	{
		ProtocolType pt = _get_protocol_type(m.second.proto_type);
		if (pt != PT_Unknown)
		{
			Module* md = new Module(
				config::module_phase.root_path,
				m.second.name,
				m.second.bin_name,
				pt);

			if (md->init())
			{
				_modules[pt] = md;
			}
		}
	}
}

void ModuleManager::unload_all_modules()
{
	for (const auto& m : _modules)
	{
		m.second->uninit();
		delete m.second;
	}

	_modules.clear();
}
