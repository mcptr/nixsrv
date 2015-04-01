#include <string>
#include <iostream>

#include "server.hxx"

#include "nix/exception.hxx"
#include "nix/message/incoming.hxx"
#include "nix/message/outgoing.hxx"
#include "nix/route.hxx"

#include "nix/direct_handlers.hxx"

#include <iostream>

namespace nix {


Server::Server(const server::Options& options, std::shared_ptr<Logger> logger)
	: address_(options.address),
	  logger_(logger)
{
	yami::parameters params;
	params.set_integer("dispatcher_threads", options.dispatcher_threads);
	params.set_integer("tcp_nonblocking", options.tcp_nonblocking);
	params.set_integer("tcp_listen_backlog", options.tcp_listen_backlog);

	agent_.reset(new yami::agent(params));
	dispatcher_.reset(new server::Dispatcher(logger));
}

Server::~Server()
{
	logger_->log_debug("~Server()");
	agent_.reset();
}

void Server::start()
{
	// this throws yami::yami_runtime_error
	resolved_address_ = agent_->add_listener(address_);
	logger_->log_info("Listening on: " + resolved_address_);
}

void Server::stop()
{
	agent_->remove_listener(resolved_address_);
	logger_->log_debug("Removing listener: " + resolved_address_);
}

void Server::register_module(std::shared_ptr<const Module> inst)
{
	dispatcher_->add_routes(inst->get_ident(), inst->get_routing());
	logger_->log_debug("Registering object: " + inst->get_ident());
	agent_->register_object(inst->get_ident(), *dispatcher_);
}

void Server::register_object(const std::string& name,
							 DirectHandler_t handler)
{
	agent_->register_object(name, handler);
}


} // nix
