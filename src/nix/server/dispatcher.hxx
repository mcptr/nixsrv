#ifndef NIX_SERVER_DISPATCHER_HXX
#define NIX_SERVER_DISPATCHER_HXX

#include <yami4-cpp/yami.h>
#include <map>
#include <memory>
#include <atomic>

#include "nix/core/auth.hxx"
#include "nix/route.hxx"
#include "nix/module.hxx"


namespace nix {
namespace server {

class ServerStats
{
	std::atomic_ullong x;
};

class Dispatcher
{
public:
	typedef std::map<const std::string,
					 std::shared_ptr<Route>> Routing_t;

	Dispatcher(bool development_mode = false);
	virtual ~Dispatcher() = default;

	void add_routes(const std::string& module,
					const Module::Routes_t& routes);

	void add_route(const std::string& module,
				   std::shared_ptr<Route> route);

	void operator()(yami::incoming_message& msg);

protected:
	Routing_t routing_;
	nix::core::Auth auth_;
	bool development_mode_ = false;
	std::unique_ptr<ServerStats> stats_;

	std::unordered_map<
		std::string,
		std::unique_ptr<ServerStats>> stats_by_module_;
};


} // server
} // nix

#endif
