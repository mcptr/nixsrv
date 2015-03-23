#include <dlfcn.h>

#include "instance.hxx"

namespace nix {
namespace core {


ModuleInstance::ModuleInstance(ModuleAPI& api, const std::string& lib_path)
	: api_(api), lib_path_(lib_path)
{
	/* empty */
}

ModuleInstance::~ModuleInstance()
{
	std::string err;
	unload(err);
}

Module& ModuleInstance::module() const
{
	return *(module_ptr_.get());
}

bool ModuleInstance::load(std::string& err_msg)
{
	dlerror(); // clear any nss cache (FreeBSD)
	void* h = dlopen(lib_path_.c_str(), RTLD_LAZY | RTLD_NOW);
	err_msg.assign(dlerror());
	if(!h) {
		return false;
	}

	Module::create_t* creator = (Module::create_t*) dlsym(h, "create");
	err_msg.assign(dlerror());
	if(!creator) {
		return false;
	}

	module_ptr_ = creator(api_);
	lib_handle_.reset((long*)h); // fixme
	return !!module_ptr_;
}

bool ModuleInstance::unload(std::string& err_msg)
{
	err_msg.clear();
	module_ptr_.reset();
	dlclose(lib_handle_.get());
	err_msg.assign(dlerror());
	lib_handle_.release();
	return err_msg.length() == 0;
}

} // core
} // nix
