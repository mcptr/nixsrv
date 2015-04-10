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
	// FIXME: make it configurable
	std::time_t now = std::time(0);
	return (now - ctime) > 60 * 15;
}

Cache::Cache(std::shared_ptr<ModuleAPI> api,
			 const nix::server::Options& options)
	: BuiltinModule(api, "Cache", 1, options)
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

	if(key.empty()) {
		msg->fail(
			nix::data_invalid_content,
			"Cannot cache empty key/value."
		);
		LOG(DEBUG) << " CACHE FAILED store";
	}
	else {
		mtx_.lock();
		CacheEntry entry;

		int period = msg->get("period", 0);
		entry.validity_period = period ? period : entry.validity_period;	

		std::string value = msg->get_serialized("value");
		entry.content = value;

		cache_[key] = entry;
		mtx_.unlock();
		msg->reply();
	}
}

void Cache::retrieve(std::unique_ptr<IncomingMessage> msg)
{
	std::string key = msg->get("key", "");

	if(key.empty() || !cache_.count(key)) {
		msg->fail(nix::null_value, (cache_.count(key) ? "EMPTY" : "NOT FOUND"));
		LOG(DEBUG) << " CACHE FAILED retrieve " << key;
	}

	else {
		if(cache_[key].expired()) {
			cache_.erase(key);
			msg->fail(nix::null_value);
		}
		else {
			msg->clear();
			msg->set_deserialized(key, cache_[key].content);
			msg->reply();
		}
	}
}

void Cache::remove(std::unique_ptr<IncomingMessage> msg)
{
	mtx_.lock();
	cache_.erase(msg->get("key", ""));
	mtx_.unlock();
}


} // module
} // nix
