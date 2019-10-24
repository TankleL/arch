#include "module-plugin.hpp"

using namespace std;
using namespace arch;

PluginModule::PluginModule(
	const std::string& pool_path,
	const std::string& name,
	const std::string& binname,
	int protocol_number)
	: _name(name)
	, _binname(binname)
	, _fullpath(pool_path + "/" + name)
	, _proto_num(protocol_number)
	, _hmodule(nullptr)
	, _i_module_preload(nullptr)
	, _i_module_init(nullptr)
	, _i_module_uninit(nullptr)
	, _i_module_service_proc(nullptr)
{}

bool PluginModule::init()
{
	bool retval = true;

	_hmodule = osys::dll_load(_fullpath + "/" + _binname);
	if (_hmodule != osys::dll_invalid_handler)
	{
		_i_module_preload = static_cast<module_preload_fp_t>(
			osys::dll_symbol(_hmodule, "arch_module_preload"));
		retval = retval && _i_module_preload != nullptr;

		_i_module_init = static_cast<module_init_fp_t>(
			osys::dll_symbol(_hmodule, "arch_module_init"));
		retval = retval && _i_module_init != nullptr;

		_i_module_uninit = static_cast<module_uninit_fp_t>(
			osys::dll_symbol(_hmodule, "arch_module_uninit"));
		retval = retval && _i_module_uninit != nullptr;

		_i_module_service_proc = static_cast<module_service_proc_fp_t>(
			osys::dll_symbol(_hmodule, "arch_service_processor"));
		retval = retval && _i_module_service_proc != nullptr;

		if (!retval)
		{
			osys::dll_unload(_hmodule);
		}
		else
		{
			_i_module_preload();

			retval = retval && (0 == _i_module_init());
			if (!retval)
			{
				osys::dll_unload(_hmodule);
			}
		}
	}
	else
	{
		retval = false;
	}

	return retval;
}

void PluginModule::uninit()
{
	_i_module_uninit();
	osys::dll_unload(_hmodule);
}

void PluginModule::process(ArchMessage& onode, const ArchMessage& inode)
{
	assert(_i_module_service_proc);

	_i_module_service_proc(onode, inode);
}

void PluginModule::dispose() noexcept
{
	delete this;
}
