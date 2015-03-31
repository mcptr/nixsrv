#include "nix/message/incoming.hxx"
#include "dispatcher.hxx"


namespace nix {
namespace server {


Dispatcher::Dispatcher(std::shared_ptr<Logger> logger)
	: logger_(logger)
{
	logger_->log_debug("Initialized dispatcher");
}

Dispatcher::~Dispatcher()
{
	logger_->log_debug("~Dispatcher()");
}

void Dispatcher::add_routes(const std::string& module,
							const Module::Routes_t& routes)
{
	Module::Routes_t::const_iterator it = routes.begin();
	for( ; it != routes.end(); it++) {
		add_route(module, *it);
	}
}

void Dispatcher::add_route(const std::string& module,
						   std::shared_ptr<Route> route)
{
	std::string full_route = module + "::" + route->get_route();
	routing_.emplace(full_route, route);
	logger_->log_debug("Dispatcher::add_route(): " + full_route);
}

void Dispatcher::dispatch(yami::incoming_message& msg)
{
	std::string route = msg.get_object_name() + "::" + msg.get_message_name();
	Routing_t::iterator it = routing_.find(route);
	if(it != routing_.end()) {
		IncomingMessage im;
		const yami::parameters& params = msg.get_parameters();
		im.parse(params.get_string("message"), true);

		switch(it->second->get_processing_type()) {
		case Route::SYNC:
			it->second->handle(im);
			break;
		case Route::ASYNC: //thread?
			it->second->handle(im);
			break;
		case Route::DEFERRED:
			// uuid into message, and queue
			it->second->handle(im);
		default:
			msg.reject();
		}
	}
	else {
		msg.reject();
	}
}


} // server
} // nix
