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


YAMI::YAMI(const Options& options)
	: Transport<yami::incoming_message>(options)
{
	yami::parameters params;
	params.set_integer("dispatcher_threads", options_.dispatcher_threads);
	params.set_integer("tcp_nonblocking", options_.tcp_nonblocking);
	params.set_integer("tcp_listen_backlog", options_.tcp_listen_backlog);

	agent_.reset(new yami::agent(stats_callback_, params));

	agent_->register_object("stats", stats_callback_);
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
	resolved_address_ = agent_->add_listener(build_address());
}

void YAMI::stop()
{
	agent_->remove_listener(resolved_address_);
}

void YAMI::register_module(std::shared_ptr<const Module> inst)
{
	dispatcher_.set_routing(
		inst->get_ident(),
		inst->get_routing()
	);

	agent_->register_object(inst->get_ident(), dispatcher_);
}

void YAMI::register_object(
	const std::string& name,
	DirectHandler_t handler)
{
	agent_->register_object(name, handler);
}

void YAMI::register_io_error_handler(IOErrorHandler_t handler)
{
	agent_->register_io_error_logger(handler);
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
										const Transport<yami::parameters>::Routes_t& routing)
{
	Transport<yami::parameters>::Routes_t::const_iterator it = routing.begin();
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


// // YAMIEventCallback

// void YAMIEventCallback::operator()(yami::incoming_message& msg)
// {
// 	using namespace yami;
// 	if (msg.get_message_name() != "get") {
// 		msg.reject("Unknown message name.");
// 	}

// 	const parameters & msg_params = msg.get_parameters();
// 	bool reset = false;

//     parameters reply_params;
//     get(reply_params, reset);

// 	msg.reply(reply_params);
// }


} // transport
} // nix
