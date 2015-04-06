#include "module.hxx"
#include "nix/common.hxx"


namespace nix {


Module::Module(std::shared_ptr<ModuleAPI> api, const std::string& id, int version)
	: api_(api),
	  ident_(id),
	  version_(version)
{
	routes_.push_back(
		std::shared_ptr<Route>(
			new Route("list_routes",
					  std::bind(&Module::list_routes, this, _1),
					  Route::ANY,
					  Route::SYNC)));
}

Module::~Module()
{
	LOG(DEBUG) << this->get_ident();
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

void Module::start()
{
	/* default impl does nothing */
}

void Module::stop()
{
	/* default impl does nothing */
}

void Module::list_routes(std::unique_ptr<IncomingMessage> msg) const
{
	msg->clear();
	for(auto& it : routes_) {
		msg->set("routing." + it->get_route(), "");
	}
	msg->reply(*msg);
}

} // nix
