#include "nix/core/routing/route.hxx"
#include "nix/api/message/incoming.hxx"
#include "nix/api/message/outgoing.hxx"

#include "yami.hxx"


namespace nix {
namespace core {
namespace net {
namespace transport {

YAMI::YAMI(const Options& options)
	: Transport(options)
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

void YAMI::start()
{
	// this throws yami::yami_runtime_error
	address_ = agent_->add_listener(address_);
}

void YAMI::register_module(std::shared_ptr<const Module> inst)
{
	req_dispatcher_.set_routing(
		inst->get_ident(),
		inst->get_routing()
	);

	agent_->register_object(inst->get_ident(), req_dispatcher_);
}



// request handler
void YAMIRequestDispatcher::operator()(yami::incoming_message& im)
{
	YAMIRequest req(im);
}

void YAMIRequestDispatcher::set_routing(const std::string& module_name,
										const Transport::Routes_t& routing)
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



} // transport
} // net
} // core
} // nix
