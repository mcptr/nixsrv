#include <functional>
#include <thread>
#include <chrono>

#include "nix/common.hxx"
#include "nix/route.hxx"
#include "nix/message.hxx"

#include "cache.hxx"


namespace nix {
namespace module {

CacheEntry::CacheEntry()
{
	ctime = std::time(0);
}

bool CacheEntry::expired() const
{
	std::time_t now = std::time(0);
	return (now - ctime) > 0;
}


Cache::Cache(std::shared_ptr<ModuleAPI> api)
	: Module(api, "Cache", 1)
{
	using namespace std::placeholders;

	std::shared_ptr<Route> store_route(
		new Route("store", std::bind(&Cache::store, this, _1), Route::API_PRIVATE, Route::SYNC)
	);

	std::shared_ptr<Route> retrieve_route(
		new Route("retrieve", std::bind(&Cache::retrieve, this, _1), Route::API_PRIVATE, Route::SYNC)
	);

	std::shared_ptr<Route> remove_route(
		new Route("remove", std::bind(&Cache::remove, this, _1), Route::API_PRIVATE, Route::VOID)
	);

	routes_.push_back(store_route);
	routes_.push_back(retrieve_route);
	routes_.push_back(remove_route);
}

void Cache::store(std::unique_ptr<IncomingMessage> msg)
{
	std::string key = msg->get("key", "");
	std::string value = msg->get("value", "");
	if(key.empty() || value.empty()) {
		msg->reject(
			nix::data_invalid_content,
			"Cannot cache empty key/value."
		);
	}
	else {
		CacheEntry entry;
		int period = msg->get("period", 0);
		entry.validity_period = period ? period : entry.validity_period;
		entry.content = msg->to_string();
		cache_[key] = entry;
		msg->reply();
	}
}

void Cache::retrieve(std::unique_ptr<IncomingMessage> msg)
{
	std::string key = msg->get("key", "");
	if(key.empty() || !cache_.count(key)) {
		msg->reject(nix::null_value);
	}

	else {
		if(cache_[key].expired()) {
			cache_.erase(key);
			msg->reject(nix::null_value);
		}
		else {
			msg->clear();
			msg->set(key, cache_[key].content);
			msg->reply(*msg);
		}
	}
}

void Cache::remove(std::unique_ptr<IncomingMessage> msg)
{
	cache_.erase(msg->get("node", ""));
}


} // module
} // nix
