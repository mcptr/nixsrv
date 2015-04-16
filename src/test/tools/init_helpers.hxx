#ifndef NIX_TEST_TOOLS_INIT_HELPERS_HXX
#define NIX_TEST_TOOLS_INIT_HELPERS_HXX

#include "constants.hxx"

#include <nix/core/service_client.hxx>
#include <memory>


namespace test {


template<class T>
std::unique_ptr<T>
init_service_client(const std::string& local_address,
					const std::string& local_nodename)
{
	nix::core::ClientConfig config;
	config.local_nodename = local_nodename;
	config.local_address = local_address;
	config.api_key = DEVELOPMENT_KEY;

	config.srv_resolver_address = local_address;
	config.srv_cache_address = local_address;
	config.srv_job_queue_address = local_address;
	config.srv_broker_address = local_address;

	config.is_local_resolver = true;
	config.is_local_cache = true;
	config.is_local_job_queue = true;
	config.is_local_broker = true;

	std::unique_ptr<T> ptr(new T(config));
	return ptr;
}


} // test

#endif
