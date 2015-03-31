#ifndef NIX_SERVER_DISPATCHER_HXX
#define NIX_SERVER_DISPATCHER_HXX

#include <yami4-cpp/yami.h>
#include <map>
#include <memory>

#include "nix/route.hxx"
#include "nix/logger.hxx"
#include "nix/module.hxx"
#include "nix/queue.hxx"

namespace nix {
namespace server {


class Dispatcher
{
public:
	typedef std::map<const std::string,
					 std::shared_ptr<Route>> Routing_t;

	Dispatcher() = delete;
	Dispatcher(std::shared_ptr<Logger> logger);
	virtual ~Dispatcher();

	void add_routes(const std::string& module,
					const Module::Routes_t& routes);

	void add_route(const std::string& module,
				   std::shared_ptr<Route> route);

	void dispatch(yami::incoming_message& msg);

protected:
	std::shared_ptr<Logger> logger_;
	Routing_t routing_;
};


} // server
} // nix

#endif
