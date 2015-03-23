#include <memory>

#include "nix/core/object_pool.hxx"
#include "manager.hxx"

namespace nix {
namespace core {

ModuleManager::ModuleManager(ModuleAPI& api,
							 Logger& logger)
	: api_(api),
	  logger_(logger)
{
}

void ModuleManager::load(const ModuleList_t& modules)
{
	std::string err_msg;
	for(auto it : modules) {
		logger_.log_info("TODO*** ModuleManager::load(): loading module: " + it);
		ModuleInstance* mod = new ModuleInstance(api_, it);
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

}
} // core
} // nix
