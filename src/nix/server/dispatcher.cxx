#include "dispatcher.hxx"

#include "nix/common.hxx"
#include "nix/message.hxx"
#include "nix/message/incoming.hxx"


namespace nix {
namespace server {

Dispatcher::Dispatcher(bool development_mode)
	: auth_(nix::core::Auth(development_mode)),
	  development_mode_(development_mode)
{
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
	LOG(DEBUG) << full_route;
}

void Dispatcher::operator()(yami::incoming_message& msg)
{
	std::string route = msg.get_object_name() + "::" + msg.get_message_name();
	Routing_t::iterator it = routing_.find(route);

	if(it != routing_.end()) {
		const yami::parameters& msg_params = msg.get_parameters();

		bool has_message = false;
		yami::parameter_entry msg_entry;

		if(msg_params.find("message", msg_entry)) {
			has_message = true;
		}

		std::unique_ptr<IncomingMessage> im(
			new IncomingMessage(msg)
		);

		if(has_message) {
			im->parse(msg_params.get_string("message"));
		}

		std::string auth_error;
		if(!auth_.check_access(*im, *(it->second.get()), auth_error)) {
			LOG(DEBUG) <<  "AuthError: " << route << " / " << auth_error;
			im->fail(nix::auth_unauthorized, auth_error);
			return;
		}

		switch(it->second->get_processing_type()) {
		case Route::VOID:
		case Route::SYNC:
			it->second->handle(std::move(im));
			break;
		case Route::ASYNC:
			it->second->handle(std::move(im));
			break;
		case Route::FUTURE:
			// uuid into message, and queue
			it->second->handle(std::move(im));
			break;
		case Route::PUBLISH:
			// uuid into message, and queue
			//it->second->handle(im);
			msg.reply();
			break;
		default:
			LOG(DEBUG) << "Rejected: "
						<< route
						<< " / Unknown processing type";
			msg.reject();
		}
	}
	else {
		LOG(DEBUG) << "Rejected (route not found): " << route;
		std::unique_ptr<IncomingMessage> im(
			new IncomingMessage(msg)
		);

		im->fail("Route not found");
	}
}


} // server
} // nix
