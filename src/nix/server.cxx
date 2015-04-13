#include <string>
#include <functional>

#include "server.hxx"

#include "nix/common.hxx"
#include "nix/exception.hxx"
#include "nix/message/incoming.hxx"
#include "nix/message.hxx"
#include "nix/route.hxx"

#include "nix/direct_handlers.hxx"


namespace nix {


void is_node_alive_handler(yami::incoming_message& msg)
{
	msg.reply();
}


Server::Server(const server::Options& options)
	: address_(options.address)
{
	yami::parameters params;
	params.set_integer("dispatcher_threads", options.dispatcher_threads);
	params.set_integer("tcp_nonblocking", options.tcp_nonblocking);
	params.set_integer("tcp_listen_backlog", options.tcp_listen_backlog);

	agent_.reset(new yami::agent(params));
	dispatcher_.reset(new server::Dispatcher(options));
}

Server::~Server()
{
	LOG(INFO) << "~Server()";
	if(is_running_) {
		stop();
	}
	agent_.reset();
}

void Server::start()
{
	using std::placeholders::_1;
	agent_->register_object(server::BUILTIN_STATUS_OBJECT_NAME,
							 *(dispatcher_.get()));

	agent_->register_object("ping", is_node_alive_handler);
	agent_->register_object("*", *(dispatcher_.get()));

	// this throws yami::yami_runtime_error
	resolved_address_ = agent_->add_listener(address_);
	is_running_ = true;
	LOG(INFO) << "Listening on: " << resolved_address_;
}

void Server::stop()
{
	agent_->remove_listener(resolved_address_);
	is_running_ = false;
	LOG(DEBUG) << "Removing listener: " << resolved_address_;
}

void Server::register_module(std::shared_ptr<const Module> inst)
{
	dispatcher_->add_routes(inst->get_ident(), inst->get_routing());
	LOG(DEBUG) << "Registering object: " << inst->get_ident();
	agent_->register_object(inst->get_ident(), *(dispatcher_.get()));
}

void Server::register_object(const std::string& name,
							 DirectHandler_t handler)
{
	agent_->register_object(name, handler);
}

} // nix
