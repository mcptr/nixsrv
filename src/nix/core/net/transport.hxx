#ifndef NIX_CORE_NET_TRANSPORT_HXX
#define NIX_CORE_NET_TRANSPORT_HXX

#include <string>
#include "nix/core/module.hxx"
#include "transport/options.hxx"

namespace nix {
namespace core {
namespace net {

class Transport
{
public:
	typedef enum { YAMI } TransportType_t;

	Transport() = delete;
	explicit Transport(const transport::Options& options);
	virtual ~Transport() = default;

	virtual void start() = 0;
	virtual void stop();
	virtual void register_module(const Module* module) = 0;
protected:
	const transport::Options& options_;
	std::string address_;
};

Transport* create_transport(Transport::TransportType_t type,
							const transport::Options& options);

} // net
} // core
} // nix

#endif
