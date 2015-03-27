#include <string>

#include "nix/impl_types.hxx"

#include "nix/exception.hxx"
#include "nix/message/incoming.hxx"
#include "nix/message/outgoing.hxx"
#include "nix/module.hxx"
#include "nix/route.hxx"

#include "yami.hxx"

#include <iostream>

namespace nix {
namespace transport {


YAMI::YAMI(const Options& options)
	: Transport<yami::incoming_message>(options)
{
	yami::parameters params;
	params.set_integer("dispatcher_threads", options_.dispatcher_threads);
	params.set_integer("tcp_nonblocking", options_.tcp_nonblocking);
	params.set_integer("tcp_listen_backlog", options_.tcp_listen_backlog);
	agent_.reset(new yami::agent(params));
}

YAMI::~YAMI()
{
	agent_.reset();
}

std::string YAMI::build_address() const
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
		throw nix::InitializationError("YAMI::build_address(): Invalid protocol");
	}

	address.append(options_.listen_address.c_str());
	address.append(":");
	address.append((options_.port ? std::to_string(options_.port) : "*"));
	return address;
}

void YAMI::start()
{
	// this throws yami::yami_runtime_error
	std::string resolved_address = agent_->add_listener(build_address());
}

void YAMI::register_module(std::shared_ptr<const Module> inst)
{
	req_dispatcher_.set_routing(
		inst->get_ident(),
		inst->get_routing()
	);

	agent_->register_object(inst->get_ident(), req_dispatcher_);
}

void YAMI::register_object(
	const std::string& name,
	std::function<void(yami::incoming_message&)> handler)
{
	agent_->register_object(name, handler);
}


// request handler
void YAMIRequestDispatcher::operator()(yami::incoming_message& im)
{
	YAMIRequest req(im);
	// h = find route
	// h
}

void YAMIRequestDispatcher::set_routing(const std::string& module_name,
										const Transport<yami::parameters>::Routes_t& routing)
{
	routing_.emplace(module_name, routing);
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


} // transport
} // nix
