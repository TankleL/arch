#include "config.hpp"
#include "modulemgr.hpp"
#include "module-plugin.hpp"
#include "module-builtin.hpp"

using namespace std;
using namespace arch;


ModuleManager::ModuleManager()
{}

ModuleManager::~ModuleManager()
{
	unload_all_modules();
}

IModule* ModuleManager::get_module(int proto_number) const
{
	const auto& mdl = _modules.find(proto_number);
	if (mdl != _modules.end())
	{
		return mdl->second;
	}
	else
	{ // no corresponding module
		assert(false);
		return nullptr;
	}
}

void ModuleManager::load_all_modules()
{
	_load_builtin_modules();
	_load_plugin_modules();
}

void ModuleManager::_load_builtin_modules()
{
	_modules[PT_Http] = new BuiltinModule_Http();
	_modules[PT_WebSocket] = new BuiltinModule_WebSocket(this);
}

void ModuleManager::_load_plugin_modules()
{
	for (const auto& m : config::module_phase.modules)
	{
		int pt = m.second.proto_type;

		IModule* md = new PluginModule(
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

void ModuleManager::unload_all_modules()
{
	for (auto& m : _modules)
	{
		m.second->uninit();
		m.second->dispose();
	}

	_modules.clear();
}


