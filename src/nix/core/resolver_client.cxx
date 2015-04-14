#include "resolver_client.hxx"


namespace nix {
namespace core {


ResolverClient::ResolverClient(const std::string& server_address,
							   const std::string& self_nodename,
							   const std::string& api_key,
							   size_t max_timeout_ms_)
	: ServiceClient("Resolver", server_address, api_key, max_timeout_ms_),
	  self_nodename_(self_nodename)
{
}

// FIXME: get address from server options
bool ResolverClient::bind_node(const std::string& address)
{
	Message m;
	m.set("nodename", self_nodename_);
	m.set("address", address);
	auto const& response = this->call("node/bind", m);
	return response->is_status_ok();
}

bool ResolverClient::bind_service(const std::string& service)
{
	Message m;
	m.set("service", service);
	m.set("nodename", self_nodename_);
	auto const& response = this->call("service/bind", m);
	return response->is_status_ok();
}

std::string ResolverClient::resolve_node(const std::string& nodename)
{
	Message m;
	m.set("nodename", nodename);
	auto const& response = this->call("node/resolve", m);
	return response->get(nodename, "");
}

Message::Array_t ResolverClient::resolve_service(const std::string& service)
{
	Message m;
	m.set("service", service);
	auto const& response = this->call("service/resolve", m);
	return response->get("addresses", Message::Array_t());
}

bool ResolverClient::unbind_node()
{
	Message m;
	m.set("nodename", self_nodename_);
	auto const& response = this->call("node/unbind", m);
	return response->is_status_ok();
}

bool ResolverClient::unbind_service(const std::string& service)
{
	Message m;
	m.set("service", service);
	m.set("nodename", self_nodename_);
	auto const& response = this->call("service/unbind", m);
	return response->is_status_ok();
}


} // core
} // nix
