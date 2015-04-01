#include <memory>

#include "nix/object_pool.hxx"
#include "manager.hxx"


namespace nix {


ModuleManager::ModuleManager(std::shared_ptr<ModuleAPI> api,
			  std::shared_ptr<Logger> logger,
			  bool fatal)
	: api_(api),
	  logger_(logger),
	  fatal_(fatal)
{
}

ModuleManager::~ModuleManager()
{
	logger_->log_debug("~ModuleManager()");
	for(auto& it : builtins_) {
		logger_->log_debug("Deleting builtin module: " + it->get_ident());
		it.reset();
	}

	modules_pool_.clear();
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

void ModuleManager::add_builtin(std::shared_ptr<Module> module)
{
	builtins_.push_back(module);
}


void ModuleManager::register_routing(std::shared_ptr<nix::Server> server)
{
	for(auto& it : builtins_) {
		logger_->log_debug(
			"ModuleManager::register_routing(): " +
			it->get_ident()
		);
		server->register_module(it);
	}

	modules_pool_.apply(
		[this, &server] (ModuleInstance& inst)
		{ 
			this->logger_->log_debug(
				"ModuleManager::register_routing(): " +
				inst.module()->get_ident()
			);
			server->register_module(inst.module());
		}
	);
}


} // nix
