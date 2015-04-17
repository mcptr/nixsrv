#ifndef NIX_MODULE_BUILTIN_SERVICE_CACHE_HXX
#define NIX_MODULE_BUILTIN_SERVICE_CACHE_HXX

#include <memory>
#include <unordered_map>
#include <mutex>

// in c++14
//#include <shared_mutex>
//
#include <thread>
#include <atomic>
#include "nix/module/builtin.hxx"
#include "nix/message/incoming.hxx"


namespace nix {
namespace module {


class CacheEntry
{
public:
	CacheEntry();
	bool expired() const;

	int ctime;
	// FIXME: read this from config for builtins
	int max_age = 5 * 60; // seconds
	std::string content;
};

class Cache : public BuiltinModule
{
public:
	Cache() = delete;
	virtual ~Cache() = default;
	explicit Cache(std::shared_ptr<ModuleAPI> api,
				   const nix::server::Options& options);

	void start();
	void stop();

private:
	void store(std::unique_ptr<IncomingMessage> msg);
	void retrieve(std::unique_ptr<IncomingMessage> msg);
	void remove(std::unique_ptr<IncomingMessage> msg);
	void status(std::unique_ptr<IncomingMessage> msg);

	std::unordered_map<std::string, CacheEntry> cache_;

	std::atomic_ullong stat_writes_{0};
	std::atomic_ullong stat_writes_failed_{0};
	std::atomic_ullong stat_removals_{0};
	std::atomic_ullong stat_hits_{0};
	std::atomic_ullong stat_hits_expired_{0};
	std::atomic_ullong stat_misses_{0};

	std::atomic<bool> cleaner_stop_flag_ {false};
	std::timed_mutex cleaner_mtx_;
	std::thread cleaner_thread_;
	void cleaner();
};


} // module
} // nix

#endif
