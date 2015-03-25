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

void YAMI::register_module(const Module* module)
{
	//agent_->register_object(module->get_ident(), []() { });
}

} // transport
} // net
} // core
} // nix
