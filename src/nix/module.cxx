#include "module.hxx"
#include "nix/common.hxx"
#include "nix/types.hxx"


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
					  Route::SYNC,
					  "Display all routes handled by '" + ident_ + "' module"
			)
		)
	);

	routes_.push_back(
		std::shared_ptr<Route>(
			new Route("ping",
					  std::bind(&Module::is_alive, this, _1),
					  Route::ANY,
					  Route::SYNC
			)
		)
	);
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
	msg->set("module.ident", ident_);
	msg->set("module.version", version_);
	for(auto& it : routes_) {
		const std::string prefix = "routing." + it->get_route();
		msg->set(prefix + ".access_modifier", 
				 str_access_modifier(it->get_access_modifier()));

		msg->set(prefix + ".processing_type",
				 str_processing_type(it->get_processing_type()));

		msg->set(prefix + ".description", it->get_description());
	}

	LOG(DEBUG) << "Replying to list_routes";
	msg->reply(*msg);
}

void Module::is_alive(std::unique_ptr<IncomingMessage> msg) const
{
	msg->reply();
}


} // nix
