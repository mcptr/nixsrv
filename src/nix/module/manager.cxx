#include <memory>

#include "nix/common.hxx"
#include "nix/object_pool.hxx"
#include "manager.hxx"


namespace nix {


ModuleManager::ModuleManager(std::shared_ptr<ModuleAPI> api,
			  bool fatal)
	: api_(api),
	  fatal_(fatal)
{
}

ModuleManager::~ModuleManager()
{
	stop_all();
	for(auto& it : builtins_) {
		LOG(DEBUG) << "Deleting builtin module: " << it->get_ident();
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

void ModuleManager::start_all()
{
	for(auto& it : builtins_) {
		LOG(DEBUG) << "Starting builtin module: " << it->get_ident();
		it->start();
	}

	modules_pool_.apply(
		[] (ModuleInstance& inst) 
		{ 
			LOG(DEBUG) << "Starting module: " 
					   << inst.module()->get_ident();
			inst.module()->start();
		}
	);

	running_ = true;
}

void ModuleManager::stop_all()
{
	if(!running_) {
		return;
	}

	for(auto& it : builtins_) {
		LOG(DEBUG) << "Stoping builtin module: "  << it->get_ident();
		it->stop();
	}

	modules_pool_.apply(
		[] (ModuleInstance& inst)
		{ 
			LOG(DEBUG) << "StartingSopting module: "
					   << inst.module()->get_ident();
			inst.module()->stop();
		}
	);

	running_ = false;
}


void ModuleManager::add_builtin(std::shared_ptr<Module> module)
{
	builtins_.push_back(module);
}


void ModuleManager::register_routing(std::shared_ptr<nix::Server> server)
{
	for(auto& it : builtins_) {
		LOG(DEBUG) << it->get_ident();
		server->register_module(it);
	}

	modules_pool_.apply(
		[&server] (ModuleInstance& inst)
		{ 
			LOG(DEBUG) << inst.module()->get_ident();
			server->register_module(inst.module());
		}
	);
}


} // nix
