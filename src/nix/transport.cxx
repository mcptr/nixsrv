#include "nix/exception.hxx"
#include "transport.hxx"
#include "transport/yami.hxx"


namespace nix {


Transport::Transport(const transport::Options& options)
	: options_(options)
{
	/* empty */
}

void Transport::stop()
{
	/* empty */
}


namespace transport {

Transport* create_transport(Transport::TransportType_t type,
							const transport::Options& options)
{
	Transport* ptr = nullptr;
	switch(type) {
	case Transport::YAMI:
		ptr = new transport::YAMI(options);
		break;
	default:
		throw nix::FatalError("create_transport(): Invalid transport");
	}

	return ptr;
}

} // transport


} // nix
