#ifndef NIX_CORE_NET_TRANSPORT_YAMI_HXX
#define NIX_CORE_NET_TRANSPORT_YAMI_HXX

#include <string>
#include <yami4-cpp/yami.h>

#include "nix/core/module.hxx"
#include "nix/core/net/transport.hxx"
#include "options.hxx"

namespace nix {
namespace core {
namespace net {
namespace transport {

class YAMI : public Transport
{
public:
	YAMI(const Options& options);
	~YAMI();
	void start();
	void register_module(const Module* module);
private:
	std::unique_ptr<yami::agent> agent_;
};

} // transport
} // net
} // core
} // nix

#endif
