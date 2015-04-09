#ifndef NIX_SERVER_DISPATCHER_HXX
#define NIX_SERVER_DISPATCHER_HXX

#include <yami4-cpp/yami.h>
#include <map>
#include <memory>
#include <atomic>

#include "nix/core/auth.hxx"
#include "nix/route.hxx"
#include "nix/module.hxx"
#include "nix/message/incoming.hxx"
#include "nix/server/options.hxx"


namespace nix {
namespace server {


const std::string BUILTIN_STATUS_OBJECT_NAME = "Status";


struct ServerStats
{
	std::atomic_ullong requests { 0 };
	std::atomic_ullong auth_errors { 0 };
	std::atomic_ullong unroutable { 0 };
	std::atomic_ullong rejected { 0 };
};

class Dispatcher
{
public:
	typedef std::map<const std::string,
					 std::shared_ptr<Route>> Routing_t;

	Dispatcher(const nix::server::Options& options);
	virtual ~Dispatcher() = default;

	void add_routes(const std::string& module,
					const Module::Routes_t& routes);

	void add_route(const std::string& module,
				   std::shared_ptr<Route> route);

	void operator()(yami::incoming_message& msg);

	void server_status(std::unique_ptr<IncomingMessage> msg);
protected:
	Routing_t routing_;
	nix::core::Auth auth_;
	Options options_;

	std::unique_ptr<ServerStats> stats_;
	std::unordered_map<
		std::string,
		std::unique_ptr<ServerStats>> stats_by_module_;
};


} // server
} // nix

#endif
