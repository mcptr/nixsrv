#include "module.hxx"


namespace nix {


Module::Module(std::shared_ptr<ModuleAPI> api, const std::string& id, int version)
	: api_(api),
	  ident_(id),
	  version_(version)
{
}

Module::~Module()
{
	api_->logger->log_debug("~Module(): " + this->get_ident());
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
}


} // nix
