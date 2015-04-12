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

	// nodes
	std::shared_ptr<Route> bind_route(
		new Route("bind", std::bind(&Resolver::bind, this, _1),
				  Route::API_PRIVATE, Route::SYNC,
				  "Bind node name to address"
		)
	);

	std::shared_ptr<Route> resolve_route(
		new Route("resolve", std::bind(&Resolver::resolve, this, _1),
				  Route::API_PRIVATE, Route::SYNC,
				  "Resolve node's address"
		)
	);

	std::shared_ptr<Route> unbind_route(
		new Route("unbind", std::bind(&Resolver::unbind, this, _1),
				  Route::API_PRIVATE, Route::SYNC,
				  "Unbind node's address"
		)
	);

	routes_.push_back(bind_route);
	routes_.push_back(resolve_route);
	routes_.push_back(unbind_route);

	// services
	std::shared_ptr<Route> bind_service_route(
		new Route("service/bind",
				  std::bind(&Resolver::bind_service, this, _1),
				  Route::API_PRIVATE, Route::SYNC,
				  "Register a node that serves given implementation "
		)
	);

	std::shared_ptr<Route> resolve_service_route(
		new Route("service/resolve",
				  std::bind(&Resolver::resolve_service, this, _1),
				  Route::API_PRIVATE, Route::SYNC,
				  "Resolve a node that serves given implementation "
		)
	);

	std::shared_ptr<Route> unbind_service_route(
		new Route("service/unbind",
				  std::bind(&Resolver::unbind_service, this, _1),
				  Route::API_PRIVATE, Route::SYNC,
				  "Unbind a node that serves given implementation "
		)
	);

	routes_.push_back(bind_service_route);
	routes_.push_back(resolve_service_route);
	routes_.push_back(unbind_service_route);
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
	std::string nodename = msg->get("nodename", "");

	std::unique_lock<std::mutex> lock(mtx_);
	nodes_.erase(nodename);

	for(auto& it : services_) {
		it.second.erase(nodename);
	}

	lock.unlock();
	msg->reply();
}


void Resolver::bind_service(std::unique_ptr<IncomingMessage> msg)
{
	std::string service = msg->get("service", "");
	std::string nodename = msg->get("nodename", "");
	if(service.empty() || nodename.empty() ||
	   !nodes_.count(nodename)) 
	{
		msg->fail(
			nix::data_invalid_content,
			"Cannot bind a service. Invalid content or unknown node."
		);
	}
	else {
		std::unique_lock<std::mutex> lock(mtx_);
		services_[service].insert(nodename);
		lock.unlock();
		msg->clear();
		msg->reply();
	}
}

void Resolver::resolve_service(std::unique_ptr<IncomingMessage> msg)
{
	std::string service = msg->get("service", "");
	LOG(DEBUG) << "RESOLVE SER " << service;
	std::unique_lock<std::mutex> lock(mtx_);
	std::set<std::string> resolved_nodes;
	for(auto& it : services_[service]) {
		LOG(DEBUG) << "SERVICE[srv] " << it;
		resolved_nodes.insert(nodes_[it]);
	}
	lock.unlock();

	if(resolved_nodes.empty()) {
		msg->fail(nix::fail, "Unknown service.");
	}
	else {
		msg->clear();
		msg->set_array("addresses");
		for(auto& it : resolved_nodes) {
			LOG(DEBUG) << "IT " << it;
			if(!it.empty()) {
				msg->append("addresses", it);
			}
		}
		msg->reply(*msg);
	}
}

void Resolver::unbind_service(std::unique_ptr<IncomingMessage> msg)
{
	std::string service = msg->get("service", "");
	std::string nodename = msg->get("nodename", "");

	if(service.empty() || nodename.empty()) {
		msg->fail(nix::data_invalid_content);
	}
	else {
		std::unique_lock<std::mutex> lock(mtx_);
		services_[service].erase(nodename);
		lock.unlock();
		msg->reply();
	}
}


} // module
} // nix
