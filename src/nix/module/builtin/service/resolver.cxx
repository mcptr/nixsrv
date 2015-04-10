#include <functional>
#include <thread>
#include <chrono>

#include "nix/common.hxx"
#include "nix/route.hxx"
#include "nix/message.hxx"

#include "resolver.hxx"


namespace nix {
namespace module {


Resolver::Resolver(std::shared_ptr<ModuleAPI> api,
				   const nix::server::Options& options)
	: BuiltinModule(api, "Resolver", 1, options)
{
	using namespace std::placeholders;

	std::shared_ptr<Route> bind_route(
		new Route("bind", std::bind(&Resolver::bind, this, _1), Route::API_PRIVATE, Route::SYNC)
	);

	std::shared_ptr<Route> resolve_route(
		new Route("resolve", std::bind(&Resolver::resolve, this, _1), Route::API_PRIVATE, Route::SYNC)
	);

	std::shared_ptr<Route> unbind_route(
		new Route("unbind", std::bind(&Resolver::unbind, this, _1), Route::API_PRIVATE, Route::SYNC)
	);

	routes_.push_back(bind_route);
	routes_.push_back(resolve_route);
	routes_.push_back(unbind_route);
}

void Resolver::bind(std::unique_ptr<IncomingMessage> msg)
{
	std::string node = msg->get("nodename", "");
	std::string address = msg->get("address", "");
	if(node.empty() || address.empty()) {
		msg->fail(
			nix::data_invalid_content,
			"Resolver cannot bind. Invalid content."
		);
	}
	else {
		std::unique_lock<std::mutex> lock(mtx_);
		nodes_[node] = address;
		lock.unlock();
		msg->clear();
		msg->reply();
	}
}

void Resolver::resolve(std::unique_ptr<IncomingMessage> msg)
{
	std::string node = msg->get("nodename", "");

	std::unique_lock<std::mutex> lock(mtx_);
	std::string address = nodes_[node];
	lock.unlock();

	if(address.empty()) {
		msg->fail(nix::fail, "Unknown node.");
	}
	else {
		msg->clear();
		msg->set(node, address);
		msg->reply(*msg);
	}
}

void Resolver::unbind(std::unique_ptr<IncomingMessage> msg)
{
	std::unique_lock<std::mutex> lock(mtx_);
	nodes_.erase(msg->get("nodename", ""));
	lock.unlock();
	msg->reply();
}


} // module
} // nix
