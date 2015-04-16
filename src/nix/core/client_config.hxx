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

	std::string api_key = "";

	std::string srv_resolver_address = "";
	std::string srv_cache_address = "";
	std::string srv_job_queue_address = "";
	std::string srv_broker_address = "";

	bool is_local_resolver = false;
	bool is_local_cache = false;
	bool is_local_job_queue = false;
	bool is_local_broker = false;
};


} // core
} // nix

#endif
