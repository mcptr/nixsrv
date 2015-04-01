#include "dispatcher.hxx"

#include "nix/message/incoming.hxx"
#include "nix/message/outgoing.hxx"
#include "nix/message/object.hxx"
#include "nix/error_codes.hxx"


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

void Dispatcher::operator()(yami::incoming_message& msg)
{
	std::string route = msg.get_object_name() + "::" + msg.get_message_name();
	Routing_t::iterator it = routing_.find(route);
	if(it != routing_.end()) {
		std::shared_ptr<IncomingMessage> im(
			new IncomingMessage(msg)
		);

		const yami::parameters& msg_params = msg.get_parameters();

		yami::parameter_entry msg_entry;
		if(msg_params.find("message", msg_entry)) {
			im->parse(msg_params.get_string("message"), true);
		}

		std::string auth_error;
		if(!auth_.check_access(*im, *(it->second.get()), auth_error)) {
			OutgoingMessage out_msg("AuthError");
			out_msg.set_error_code(nix::error_code::auth_unauthorized);
			out_msg.set_error_msg(auth_error);

			im->reply(out_msg);

			logger_->log_debug(
				"AuthError: " + route + " / " + auth_error);

			return;
		}

		switch(it->second->get_processing_type()) {
		case Route::SYNC:
			logger_->log_debug("SYNC: " + route);
			it->second->handle(im);
			break;
		case Route::ASYNC:
			logger_->log_debug("SYNC: " + route);
			it->second->handle(im);
			break;
		case Route::FUTURE:
			// uuid into message, and queue
			logger_->log_debug("FUTURE: " + route);
			it->second->handle(im);
			break;
		case Route::PUBLISH:
			// uuid into message, and queue
			//it->second->handle(im);
			logger_->log_debug("PUBLISH: " + route + " / FIXME*******");
			msg.reply();
			break;
		default:
			logger_->log_debug("Rejected: " + route + " / Unknown processing type");
			msg.reject();
		}
	}
	else {
		msg.reject();
		logger_->log_debug("Rejected: " + route);
	}
}


} // server
} // nix
