#include <string>

#include "yami.hxx"

#include "nix/impl_types.hxx"
#include "nix/exception.hxx"
#include "nix/message/incoming.hxx"
#include "nix/message/outgoing.hxx"
#include "nix/module.hxx"
#include "nix/route.hxx"

#include "nix/direct_handlers.hxx"

#include <iostream>

namespace nix {
namespace transport {


YAMIServer::YAMIServer(const Options& options)
	: options_(options)
{
	yami::parameters params;
	params.set_integer("dispatcher_threads", options_.dispatcher_threads);
	params.set_integer("tcp_nonblocking", options_.tcp_nonblocking);
	params.set_integer("tcp_listen_backlog", options_.tcp_listen_backlog);

	agent_.reset(new yami::agent(stats_callback_, params));
	agent_->register_object("stats", stats_callback_);
}

YAMIServer::~YAMIServer()
{
	agent_.reset();
}

std::string YAMIServer::build_address() const
{
	std::string address;
	switch(options_.address_family) {
	case Options::UNIX:
		address.append("unix://");
		break;
	case Options::TCP:
		address.append("tcp://");
		break;
	case Options::UDP:
		address.append("udp://");
		break;
	default:
		throw nix::InitializationError("YAMIServer::build_address(): Invalid protocol");
	}

	address.append(options_.listen_address.c_str());
	address.append(":");
	address.append((options_.port ? std::to_string(options_.port) : "*"));
	return address;
}

void YAMIServer::start()
{
	// this throws yami::yami_runtime_error
	resolved_address_ = agent_->add_listener(build_address());
}

void YAMIServer::stop()
{
	agent_->remove_listener(resolved_address_);
}

void YAMIServer::register_module(std::shared_ptr<const Module> inst)
{
	dispatcher_.set_routing(
		inst->get_ident(),
		inst->get_routing()
	);

	agent_->register_object(inst->get_ident(), dispatcher_);
}

void YAMIServer::register_object(
	const std::string& name,
	DirectHandler_t handler)
{
	agent_->register_object(name, handler);
}



// request handler
void YAMIRequestDispatcher::operator()(yami::incoming_message& im)
{
	YAMIRequest req(im);
	Routing_t::iterator it = routing_.find(req.get_module() + ":" + req.get_route());
	if(it != routing_.end()) {
		Response res(req.get_seq_id());
		it->second->handle(req, res);
	}
}

void YAMIRequestDispatcher::set_routing(const std::string& module_name,
										const ServerTransport<yami::parameters>::Routes_t& routing)
{
	ServerTransport<yami::parameters>::Routes_t::const_iterator it = routing.begin();
	for( ; it != routing.end(); it++) {
		routing_.emplace(module_name + "::" + (*it)->get_route(), *it);
	}
}

// request

YAMIRequest::YAMIRequest(yami::incoming_message& im)
	: Request<yami::incoming_message>(im)
{
	module_ = im.get_object_name();
	route_ = im.get_message_name();

	yami::parameter_entry pe;
	if(im.get_parameters().find("message", pe)) {
		message_->parse(im.get_parameters().get_string("message"));
	}
}

void YAMIRequest::reply(nix::Response& response)
{
}



// client
YAMIClient::YAMIClient(const std::string& address,
					   const transport::Options& options)
	: ClientTransport<yami::parameters>(address, options)
{
	yami::parameters params;
	params.set_integer("tcp_nonblocking", options_.tcp_nonblocking);
	agent_.reset(new yami::agent(params));
}

YAMIClient::~YAMIClient()
{
	agent_.reset();
}

std::shared_ptr<nix::IncomingMessage> 
YAMIClient::call(const std::string& module,
				 const std::string& route,
				 const yami::parameters& parameters,
				 int timeout)
{
	std::shared_ptr<nix::IncomingMessage> msg
		= agent_->send(address_, module, route, parameters);

}

std::shared_ptr<IncomingMessage> 
YAMIClient::call(const std::string& module,
				 const std::string& route,
				 OutgoingMessage& msg,
				 int timeout)
{
	yami::parameters content;
	content.set_string("message", msg.to_string());
	call(module, route, content, timeout);
}

void YAMIClient::send_one_way(const std::string& module,
							  const std::string& route,
							  const yami::parameters& msg)
{
	agent_->send_one_way(address_, module, route, msg);
}


void YAMIClient::send_one_way(const std::string& module,
							  const std::string& route,
							  OutgoingMessage& msg)
{
	yami::parameters content;
	content.set_string("message", msg.to_string());
	send_one_way(module, route, content);
}


} // transport
} // nix
