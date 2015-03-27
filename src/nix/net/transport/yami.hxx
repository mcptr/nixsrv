#ifndef NIX_TRANSPORT_YAMI_HXX
#define NIX_TRANSPORT_YAMI_HXX

#include <string>
#include <yami4-cpp/yami.h>

#include "nix/module.hxx"
#include "nix/module/instance.hxx"
#include "nix/net/transport.hxx"
#include "nix/request.hxx"
#include "nix/routing/route.hxx"
#include "nix/response.hxx"

#include "options.hxx"


namespace nix {
namespace transport {


class YAMIRequest : nix::Request<yami::incoming_message>
{
public:
	YAMIRequest() = delete;
	YAMIRequest(yami::incoming_message& im);
	void reply(nix::Response& response);
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
} // nix

#endif
