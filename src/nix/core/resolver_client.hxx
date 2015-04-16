#ifndef NIX_CORE_RESOLVER_CLIENT_HXX
#define NIX_CORE_RESOLVER_CLIENT_HXX

#include <string>
#include "service_client.hxx"
#include "client_config.hxx"
#include "nix/message.hxx"


namespace nix {
namespace core {


class ResolverClient : public ServiceClient
{
public:
	ResolverClient() = delete;

	ResolverClient(const ClientConfig& config,
				   size_t max_timeout_ms_ = 2000);

	virtual ~ResolverClient() = default;


	// FIXME: get address from server options
	bool bind_node();

	bool bind_service(const std::string& service);

	std::string resolve_node(const std::string& nodename);

	Message::Array_t resolve_service(const std::string& service);

	bool unbind_node();

	bool unbind_service(const std::string& service);

protected:
};


} // core
} // nix

#endif
