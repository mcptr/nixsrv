#include <functional>

#include "dispatcher.hxx"

#include "nix/common.hxx"
#include "nix/message.hxx"
#include "nix/message/incoming.hxx"


namespace nix {
namespace server {


Dispatcher::Dispatcher(const nix::server::Options& options)
	: auth_(nix::core::Auth(options.development_mode)),
	  options_(options)
{
	stats_.reset(new ServerStats());

	using std::placeholders::_1;

	std::shared_ptr<Route> server_status_route(
		new Route("status",
				  std::bind(&Dispatcher::server_status, this, _1),
				  Route::API_PRIVATE,
				  Route::SYNC
		)
	);

	add_route(BUILTIN_STATUS_OBJECT_NAME, server_status_route);
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
	if(stats_by_module_.count(module) == 0) {
		std::unique_ptr<ServerStats> stats(new ServerStats());
		stats_by_module_.emplace(module, std::move(stats));
	}

	LOG(DEBUG) << full_route;
}

void Dispatcher::operator()(yami::incoming_message& msg)
{
	std::string module = msg.get_object_name();
	std::string route = module + "::" + msg.get_message_name();
	LOG(DEBUG) << "Dispatching: " << route;

	Routing_t::iterator it = routing_.find(route);

	stats_->requests++;

	if(it == routing_.end()) {
		LOG(DEBUG) << "Rejected (route not found): " << route;
		msg.reject();

		stats_->unroutable++;
		if(stats_by_module_.count(module)) {
			stats_by_module_[module]->unroutable++;
		}
	}
	else {
		stats_by_module_[module]->requests++;
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
			stats_->auth_errors++;
			stats_by_module_[module]->auth_errors++;
			return;
		}

		switch(it->second->get_processing_type()) {
		case Route::VOID:
		case Route::SYNC:
		case Route::ASYNC:
		case Route::FUTURE:
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
			stats_->unroutable++;
			stats_by_module_[module]->unroutable++;
		}
	}
}

void Dispatcher::server_status(std::unique_ptr<IncomingMessage> msg)
{
	Message om;
	
	om.set("nodename", options_.nodename);

	if(msg->get("server", false)) {
		int uptime_sec = std::time(nullptr) - options_.start_time;
		om.set("server.uptime", uptime_sec);
	}

	if(msg->get("stats", false)) {
		om.set("stats.requests", (long long)stats_->requests);
		om.set("stats.auth_errors", (long long)stats_->auth_errors);
		om.set("stats.unroutable", (long long)stats_->unroutable);
		om.set("stats.rejected", (long long)stats_->rejected);
	}

	if(msg->get("module_stats", false)) {
		for(auto& it : stats_by_module_) {
			std::string prefix = "stats." + it.first;
			om.set(prefix + ".requests", (long long)it.second->requests);
			om.set(prefix + ".auth_errors", (long long)it.second->auth_errors);
			om.set(prefix + ".unroutable", (long long)it.second->unroutable);
			om.set(prefix + ".rejected", (long long)it.second->rejected);
		}
	}

	if(msg->get("routing", false)) {
		for(auto& it : routing_) {
			std::string route_key = "server.routing." + it.first;
			om.set(route_key + ".access_modifier",
				   it.second->get_access_modifier());
			om.set(route_key + ".processing_type",
				   it.second->get_processing_type());
			om.set(route_key + ".description",
				   it.second->get_description());
		}
	}

	if(msg->get("reset", false)) {
		LOG(INFO) << "Resetting server statistics";
		stats_->requests = 0;
		stats_->auth_errors = 0;
		stats_->rejected = 0;
		stats_->unroutable = 0;

		for(auto& it : stats_by_module_) {
			it.second->requests = 0;
			it.second->auth_errors = 0;
			it.second->rejected = 0;
			it.second->unroutable = 0;
		}
	}
	msg->reply(om);
}

void Dispatcher::default_route_handler(yami::incoming_message& msg)
{
	stats_->unroutable++;
	msg.reject("Unroutable");
}


} // server
} // nix
