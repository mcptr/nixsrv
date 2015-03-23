#include <iostream>
#include "module.hxx"

namespace nix {
namespace core {

Module::Module(ModuleAPI& api, const std::string& id, int version)
	: api_(api),
	  ident_(id),
	  version_(version)
{
}

const std::string& Module::get_ident() const
{
	return this->ident_;
}

int Module::get_version() const
{
	return this->version_;
}

void Module::deleter(Module *ptr)
{
	delete ptr;
	std::cout << "deleter()" << std::endl;
}

} // core
} // nix
