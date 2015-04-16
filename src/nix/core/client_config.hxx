#ifndef NIX_CORE_CLIENT_CONFIG_HXX
#define NIX_CORE_CLIENT_CONFIG_HXX

#include <string>


namespace nix {
namespace core {


class ClientConfig
{
public:
	std::string local_nodename = "";
	std::string local_address = "";

	std::string api_key_private = "";
	std::string api_key_public = "";

	std::string srv_resolver_address = "";
	std::string srv_cache_address = "";
	std::string srv_job_queue_address = "";
	std::string srv_broker_address = "";

	bool is_local_resolver = false;
	bool is_local_cache = false;
	bool is_local_job_queue = false;
	bool is_local_broker = false;

	size_t generic_client_pool_size = 5;
	size_t resolver_client_pool_size = 5;
	size_t cache_client_pool_size = 5;
	size_t job_queue_client_pool_size = 5;
	size_t broker_client_pool_size = 5;
};


} // core
} // nix

#endif
