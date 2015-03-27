#ifndef NIX_CORE_NET_TRANSPORT_YAMI_HXX
#define NIX_CORE_NET_TRANSPORT_YAMI_HXX

#include <string>
#include <yami4-cpp/yami.h>

#include "nix/core/module.hxx"
#include "nix/core/module/instance.hxx"
#include "nix/core/net/transport.hxx"
#include "nix/core/request.hxx"
#include "nix/core/routing/route.hxx"

#include "options.hxx"

namespace nix {
namespace core {
namespace net {
namespace transport {


class YAMIRequest : nix::core::Request<yami::incoming_message>
{
public:
	YAMIRequest() = delete;
	YAMIRequest(yami::incoming_message& im);
};


class YAMIRequestDispatcher
{
public:
	YAMIRequestDispatcher() = default;

	void operator()(yami::incoming_message& im);

	void set_routing(const std::string& module_name,
				   const Transport::Routes_t& routing);

private:
	std::map<const std::string, const Transport::Routes_t&> routing_;
};


class YAMI : public Transport
{
public:
	YAMI() = delete;
	explicit YAMI(const Options& options);
	virtual ~YAMI();

	void start();
	void register_module(std::shared_ptr<const Module> inst);

private:
	std::unique_ptr<yami::agent> agent_;
	YAMIRequestDispatcher req_dispatcher_;
};

} // transport
} // net
} // core
} // nix

#endif
