#include <functional>
#include <thread>
#include <chrono>
#include <ctime>

#include "nix/common.hxx"
#include "nix/route.hxx"
#include "nix/message.hxx"
#include "nix/util/test.hxx"

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
	return (now - ctime) >= max_age;
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

	std::shared_ptr<Route> status_route(
		new Route("status", std::bind(&Cache::status, this, _1), Route::API_PRIVATE, Route::SYNC)
	);

	routes_.push_back(store_route);
	routes_.push_back(retrieve_route);
	routes_.push_back(remove_route);
	routes_.push_back(status_route);
}

void Cache::start()
{
	if(options_.cache_cleaner_enabled) {
		cleaner_stop_flag_ = false;
		cleaner_mtx_.lock();
		cleaner_thread_ = std::move(
			std::thread(std::bind(&Cache::cleaner, this)));
	}
}

void Cache::stop()
{
	if(options_.cache_cleaner_enabled) {
		cleaner_stop_flag_ = true;
		cleaner_mtx_.unlock();
		cleaner_thread_.join();
	}
}

void Cache::store(std::unique_ptr<IncomingMessage> msg)
{
#if defined(DEBUG_BUILD)
	if(options_.development_mode) {
		nix::util::test::random_thread_sleep();
	}
#endif
	std::string key = msg->get("key", "");

	if(key.empty()) {
		msg->fail(
			nix::data_invalid_content,
			"Cannot cache empty key/value."
		);
		stat_writes_failed_++;
	}
	else {
		CacheEntry entry;

		int max_age = msg->get("max_age", 0);
		entry.max_age = max_age ? max_age : entry.max_age;	

		std::string value = msg->to_string("value");
		entry.content = value;

		std::unique_lock<std::mutex> lock(mtx_);
		cache_[key] = entry;
		lock.unlock();

		msg->reply();
		stat_writes_++;
	}
}

void Cache::retrieve(std::unique_ptr<IncomingMessage> msg)
{
#if defined(DEBUG_BUILD)
	if(options_.development_mode) {
		nix::util::test::random_thread_sleep();
	}
#endif

	std::string key = msg->get("key", "");

	std::unique_lock<std::mutex> lock(mtx_);
	size_t found = cache_.count(key);
	lock.unlock();

	if(key.empty() || !found) {
		msg->clear();
		msg->fail(nix::null_value);
		stat_misses_++;
	}
	else {
		std::string content;

		std::unique_lock<std::mutex> lock(mtx_);
		bool expired = cache_[key].expired();
		if(expired) {
			cache_.erase(key);
		}
		else {
			content = cache_[key].content;
		}
		lock.unlock();

		if(expired) {
			msg->fail(nix::null_value);
			stat_hits_expired_++;
		}
		else {
			msg->clear();
			msg->set_deserialized(key, content);
			msg->reply(*msg);
			stat_hits_++;
		}
	}
}

void Cache::remove(std::unique_ptr<IncomingMessage> msg)
{
#if defined(DEBUG_BUILD)
	if(options_.development_mode) {
		nix::util::test::random_thread_sleep();
	}
#endif

	std::unique_lock<std::mutex> lock(mtx_);
	cache_.erase(msg->get("key", ""));
	lock.unlock();
	stat_removals_++;
}

void Cache::status(std::unique_ptr<IncomingMessage> msg)
{
#if defined(DEBUG_BUILD)
	if(options_.development_mode) {
		nix::util::test::random_thread_sleep();
	}
#endif

	msg->clear();

	msg->set("writes", (long long) stat_writes_);
	msg->set("writes_failed", (long long) stat_writes_failed_);
	msg->set("removals", (long long) stat_removals_);

	msg->set("hits", (long long) stat_hits_);
	msg->set("hits_expired", (long long) stat_hits_expired_);

	msg->set("misses", (long long) stat_misses_);

	long long total = stat_writes_ + stat_writes_failed_;
	total += stat_removals_;
	total += stat_hits_ + stat_hits_expired_ + stat_misses_;

	msg->set("total_calls", total);

	std::unique_lock<std::mutex> lock(mtx_);

	long long cache_size = cache_.size();
	msg->set("cache_size", cache_size);
	long long bytes = 0;
	for(auto& it : cache_) {
		bytes += it.second.content.size();
	}
	msg->set("bytes", bytes);
	msg->reply(*msg);

	lock.unlock();
}

void Cache::cleaner()
{
	while(!cleaner_stop_flag_) {
		LOG(INFO) << "Cache::cleaner up";
		long removed = 0;
		
		std::unique_lock<std::mutex> lock(mtx_);
		for(auto& it : cache_) {
			if(it.second.expired()) {
				cache_.erase(it.first);
				removed++;
			}
		}
		lock.unlock();
		LOG(INFO) << "Cache: Expired keys removed: " << removed;
		int loops = options_.cache_cleaner_run_interval;
		while(!cleaner_mtx_.try_lock() && loops) {
			loops--;
			std::this_thread::sleep_for(
				std::chrono::milliseconds(
					options_.cache_cleaner_sleep_interval_ms));
		}
	}
	LOG(INFO) << "Cache cleaner thread finished";
	cleaner_mtx_.unlock();
}


} // module
} // nix
