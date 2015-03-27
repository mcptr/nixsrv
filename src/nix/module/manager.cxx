#include <memory>

#include "nix/object_pool.hxx"
#include "manager.hxx"

namespace nix {
namespace core {

ModuleManager::ModuleManager(ModuleAPI& api,
							 Logger& logger,
							 bool fatal)
	: api_(api),
	  logger_(logger),
	  fatal_(fatal)
{
}

void ModuleManager::load(const Names_t& modules)
{
	std::string err_msg;
	for(auto it : modules) {
		logger_.log_info("TODO*** ModuleManager::load(): loading module: " + it);
		ModuleInstance* mod = new ModuleInstance(api_, it, fatal_);
		if(mod->load(err_msg)) {
			modules_pool_.insert(mod);
		}
		else {
			logger_.log_error("ModuleManager::load(): " + err_msg);
			delete mod;
		}
	}
}

void ModuleManager::load(const std::string& module_path)
{
	logger_.log_info("TODO*** ModuleManager::load(): loading module: " + module_path);
}

void ModuleManager::unload()
{
	modules_pool_.clear();
}

void ModuleManager::register_routing(std::shared_ptr<Transport> t)
{
	modules_pool_.apply(
		[&t] (ModuleInstance& inst)
		{ 
			t->register_module(inst.module());
		}
	);
}


} // core
} // nix
