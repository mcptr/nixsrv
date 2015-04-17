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
		new Route("node/bind", std::bind(&Resolver::bind, this, _1),
				  Route::API_PRIVATE, Route::SYNC,
				  "Bind node name to address"
		)
	);

	std::shared_ptr<Route> resolve_route(
		new Route("node/resolve", std::bind(&Resolver::resolve, this, _1),
				  Route::API_PRIVATE, Route::SYNC,
				  "Resolve node's address"
		)
	);

	std::shared_ptr<Route> unbind_route(
		new Route("node/unbind", std::bind(&Resolver::unbind, this, _1),
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

void Resolver::start()
{
	if(options_.resolver_monitor_enabled) {
		monitor_stop_flag_ = false;
		monitor_mtx_.lock();
		monitor_thread_ = std::move(
			std::thread(std::bind(&Resolver::monitor, this)));
	}
}

void Resolver::stop()
{
	if(options_.resolver_monitor_enabled) {
		monitor_stop_flag_ = true;
		monitor_mtx_.unlock();
		monitor_thread_.join();
	}
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
	unbind_node(nodename);
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
	std::unique_lock<std::mutex> lock(mtx_);
	std::set<std::string> resolved_nodes;

	for(auto& it : services_[service]) {
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

void Resolver::monitor()
{
	LOG(INFO) << "Resolver monitor thread started";

	// address, failures seen
	std::unordered_map<std::string, int> soft_fails_nodes;

	// service, adddress, failures seen
	std::unordered_map<
		std::string,
		std::unordered_map<std::string, int>> soft_fails_services;

	while(!monitor_stop_flag_) {
		LOG(INFO) << "Resolver::monitor up";

		std::unordered_map<std::string, std::string> nodes;
		std::unique_lock<std::mutex> lock(mtx_);
		nodes = nodes_;
		lock.unlock();

		yami::agent monitor_agent;
		yami::parameters empty_params;

		for(auto& node : nodes) {
			std::unique_ptr<yami::outgoing_message> om(
				monitor_agent.send(node.second, "ping", "", empty_params));
			om->wait_for_completion(
				options_.resolver_monitor_response_timeout_ms);
			const yami::message_state state = om->get_state();
			if(state == yami::replied) {
				soft_fails_nodes[node.first] = 0;
			}
			else {
				LOG(WARNING) << "Unresponsive node: " << node.first;
				int total_failures = ++soft_fails_nodes[node.first];
				if(total_failures >= options_.resolver_monitor_max_failures) {
					std::unique_lock<std::mutex> lock(mtx_);
					unbind_node(node.first);
					lock.unlock();
				}
			}
		}

		std::atomic<int> removed_services { 0 };
		std::unordered_map<std::string,
						   std::set<std::string>> services_copy;

		lock.lock();
		services_copy = services_;;
		lock.unlock();

		for(auto& s_it : services_copy) {
			for(auto& address : s_it.second) {
				std::string service = s_it.first;
				std::unique_ptr<yami::outgoing_message> om(
					monitor_agent.send(
						address, service, "ping", empty_params));
				om->wait_for_completion(
					options_.resolver_monitor_response_timeout_ms);
				const yami::message_state state = om->get_state();
				if(state == yami::replied) {
					// reset soft fails
					soft_fails_services[service][address] = 0;
				}
				else {
					LOG(WARNING) << "Unresponsive service: " << service << " @ " << address;;
					int total_failures =
						++soft_fails_services[service][address];
					if(total_failures >= 
					   options_.resolver_monitor_max_failures) 
					{
						std::unique_lock<std::mutex> lock(mtx_);
						LOG(WARNING) << "Unbinding address " << address
									 << " from service " << service
									 << ". Unresponsive node.";
						services_[service].erase(address);
						lock.unlock();
					}
				}
			}
		}

		int loops = options_.resolver_monitor_run_interval;
		while(!monitor_mtx_.try_lock() && loops) {
			loops--;
			std::this_thread::sleep_for(
				std::chrono::milliseconds(
					options_.resolver_monitor_sleep_interval_ms));
		}
	}
	LOG(INFO) << "Resolver monitor thread finished";
	monitor_mtx_.unlock();
}

void Resolver::unbind_node(const std::string& nodename)
{
	LOG(WARNING) << "Unbinding node " << nodename;
	nodes_.erase(nodename);
	for(auto& it : services_) {
		it.second.erase(nodename);
	}
}

} // module
} // nix
