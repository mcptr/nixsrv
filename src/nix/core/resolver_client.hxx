#ifndef NIX_CORE_RESOLVER_CLIENT_HXX
#define NIX_CORE_RESOLVER_CLIENT_HXX

#include <string>
#include "service_client.hxx"


namespace nix {
namespace core {


class ResolverClient : public ServiceClient
{
public:
	ResolverClient() = delete;

	ResolverClient(const std::string& server_address,
				   const std::string& api_key,
				   size_t max_timeout_ms_ = 2000);

	virtual ~ResolverClient() = default;


	bool bind_node(const std::string& nodename,
				   const std::string& address);

	bool bind_service(const std::string& service,
					  const std::string& address);

	std::string resolve_node(const std::string& nodename);

	std::string resolve_service(const std::string& service);

	bool unbind_node(const std::string& nodename);

	bool unbind_service(const std::string& service);
};


} // core
} // nix

#endif
