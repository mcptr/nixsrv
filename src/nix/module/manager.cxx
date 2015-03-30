#include <memory>

#include "nix/impl_types.hxx"
#include "nix/object_pool.hxx"
#include "manager.hxx"


namespace nix {


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
	for(auto it : modules) {
		load(it);
	}
}

void ModuleManager::load(const std::string& module_path)
{
	std::string err_msg;
	ModuleInstance* mod = new ModuleInstance(api_, module_path, fatal_);
	if(mod->load(err_msg)) {
		modules_pool_.insert(mod);
	}
	else {
		delete mod;
	}
}

void ModuleManager::unload()
{
	modules_pool_.clear();
}

void ModuleManager::register_routing(std::shared_ptr<impl::ServerTransport_t> t)
{
	modules_pool_.apply(
		[&t] (ModuleInstance& inst)
		{ 
			t->register_module(inst.module());
		}
	);
}


} // nix
