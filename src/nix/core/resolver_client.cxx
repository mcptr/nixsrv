#include "resolver_client.hxx"


namespace nix {
namespace core {


ResolverClient::ResolverClient(const ClientConfig& config,
							   size_t max_timeout_ms_)
	: ServiceClient("Resolver",
					config.srv_resolver_address,
					config,
					max_timeout_ms_)
{
}

// FIXME: get address from server options
bool ResolverClient::bind_node()
{
	Message m;
	m.set_meta("api_key", client_config_.api_key_private);
	m.set("nodename", client_config_.local_nodename);
	m.set("address", client_config_.local_address);
	auto const& response = this->call_service("node/bind", m);
	return response->is_status_ok();
}

bool ResolverClient::bind_service(const std::string& service)
{
	Message m;
	m.set_meta("api_key", client_config_.api_key_private);
	m.set("service", service);
	m.set("nodename", client_config_.local_nodename);
	auto const& response = this->call_service("service/bind", m);
	return response->is_status_ok();
}

std::string ResolverClient::resolve_node(const std::string& nodename)
{
	Message m;
	m.set_meta("api_key", client_config_.api_key_private);
	m.set("nodename", nodename);
	auto const& response = this->call_service("node/resolve", m);
	return response->get(nodename, "");
}

Message::Array_t ResolverClient::resolve_service(const std::string& service)
{
	Message m;
	m.set_meta("api_key", client_config_.api_key_private);
	m.set("service", service);
	auto const& response = this->call_service("service/resolve", m);
	return response->get("addresses", Message::Array_t());
}

bool ResolverClient::unbind_node()
{
	Message m;
	m.set_meta("api_key", client_config_.api_key_private);
	m.set("nodename", client_config_.local_nodename);
	auto const& response = this->call_service("node/unbind", m);
	return response->is_status_ok();
}

bool ResolverClient::unbind_service(const std::string& service)
{
	Message m;
	m.set_meta("api_key", client_config_.api_key_private);
	m.set("service", service);
	m.set("nodename", client_config_.local_nodename);
	auto const& response = this->call_service("service/unbind", m);
	return response->is_status_ok();
}


} // core
} // nix
