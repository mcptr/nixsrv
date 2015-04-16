#include "service_client.hxx"


namespace nix {
namespace core {


ServiceClient::ServiceClient(const std::string& service,
							 const std::string& server_address,
							 const ClientConfig& config,
							 size_t max_timeout_ms)
	: Client(config, max_timeout_ms ? max_timeout_ms : 2000),
	  service_(service),
	  server_address_(server_address),
	  client_config_(config)
{
}

bool ServiceClient::ping_service(size_t timeout_ms)
{
	auto response = this->Client::call(
		server_address_, service_, "ping", "", timeout_ms);
	return response->is_replied();
}

std::unique_ptr<nix::Response>
ServiceClient::call_service(const std::string& route,
							const std::string& msg,
							size_t timeout_ms)
{
	return this->Client::call(
		server_address_, service_, route, msg,
		timeout_ms > max_timeout_ms_ ? max_timeout_ms_ : timeout_ms);
}

bool ServiceClient::send(const std::string& route,
						 const std::string& msg)
{
	return this->Client::send_one_way(
		server_address_, service_, route, msg);
}


} // core
} // nix
