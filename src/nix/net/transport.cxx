#include "nix/exception.hxx"
#include "transport.hxx"
#include "transport/yami.hxx"

namespace nix {
namespace core {
namespace net {


Transport::Transport(const transport::Options& options)
	: options_(options)
{
	/* empty */
}

void Transport::stop()
{
	/* empty */
}

Transport* create_transport(Transport::TransportType_t type,
							const transport::Options& options)
{
	Transport* ptr = nullptr;
	switch(type) {
	case Transport::YAMI:
		ptr = new transport::YAMI(options);
		break;
	default:
		throw nix::core::FatalError("create_transport(): Invalid transport");
	}

	return ptr;
}


} // net
} // core
} // nix
