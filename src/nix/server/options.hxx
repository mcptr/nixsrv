#ifndef NIX_SERVER_OPTIONS_HXX
#define NIX_SERVER_OPTIONS_HXX

#include <string>


namespace nix {
namespace server {


class Options
{
public:
	int start_time = 0;
	std::string nodename;
	std::string address = "tcp://*:*";

	bool tcp_nonblocking = true;
	int tcp_listen_backlog = 10;
	int dispatcher_threads = 1;

	bool development_mode = false;
	bool enable_random_sleep = false;

	// module manager
	bool manager_thread_enabled = false;
	int manager_thread_run_interval = 120;
	int manager_thread_sleep_interval_ms = 120;

	// builtins.cache cleaner thread options
	bool cache_cleaner_enabled = false;
	int cache_cleaner_run_interval = 60;
	int cache_cleaner_sleep_interval_ms = 1000;

	// builtins.resolver monitor thread options
	// WARNING: modules will be removed until they
	// register themselves once again
	bool resolver_monitor_enabled = false;
	int resolver_monitor_run_interval = 60;
	int resolver_monitor_sleep_interval_ms = 1000;
	int resolver_monitor_response_timeout_ms = 2000;
	int resolver_monitor_max_failures = 3;
};


} // server
} // nix

#endif
