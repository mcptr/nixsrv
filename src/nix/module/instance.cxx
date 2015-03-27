#include <dlfcn.h>

#include "nix/exception.hxx"
#include "nix/module.hxx"
#include "instance.hxx"


namespace nix {


ModuleInstance::ModuleInstance(ModuleAPI& api,
							   const std::string& lib_path,
							   bool fatal)
	: api_(api),
	  lib_path_(lib_path),
	  fatal_(fatal)
{
	lib_handle_.reset();
}

ModuleInstance::~ModuleInstance()
{
	std::string err;
	unload(err);
}

std::shared_ptr<const Module> ModuleInstance::module() const
{
	return module_ptr_;
}

bool ModuleInstance::load(std::string& err_msg)
{
	dlerror(); // clear any nss cache (FreeBSD)
	void* h = dlopen(lib_path_.c_str(), RTLD_LAZY | RTLD_NOW);
	err_msg.assign(dlerror());
	if(!h) {
		if(fatal_) {
			throw FatalError("ModuleInstance::load(): Failed to load module: " + err_msg);
		}
		return false;
	}

	Module::create_t* creator = (Module::create_t*) dlsym(h, "create");
	err_msg.assign(dlerror());
	if(!creator) {
		return false;
	}

	module_ptr_ = creator(api_);
	lib_handle_.reset((long*)h); // FIXME
	return !!module_ptr_;
}

bool ModuleInstance::unload(std::string& err_msg)
{
	err_msg.clear();
	module_ptr_.reset();
	if(lib_handle_ && dlclose(lib_handle_.get())) {
		return false;
	}
	lib_handle_.reset();
	return true;
}


} // nix
