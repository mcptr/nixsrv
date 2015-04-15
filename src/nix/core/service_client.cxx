#include "service_client.hxx"


namespace nix {
namespace core {


ServiceClient::ServiceClient(const std::string& service,
							 const std::string& server_address,
							 const std::string& api_key,
							 size_t max_timeout_ms)
	: Client(max_timeout_ms ? max_timeout_ms : 2000),
	  service_(service),
	  server_address_(server_address),
	  api_key_(api_key)
{
}

bool ServiceClient::ping_service(size_t timeout_ms)
{
	auto response = this->Client::call(
		server_address_, service_, "ping", timeout_ms);
	return response->is_replied();
}

std::unique_ptr<nix::Response>
ServiceClient::call(const std::string& route, size_t timeout_ms)
{
	Message empty;
	return call(route, empty, timeout_ms);
}

std::unique_ptr<nix::Response>
ServiceClient::call(const std::string& route,
					Message& msg,
					size_t timeout_ms)
{
	msg.set_meta("api_key", api_key_);

	return this->Client::call(
		server_address_, service_, route, msg,
		timeout_ms > max_timeout_ms_ ? max_timeout_ms_ : timeout_ms);
}

// proxy
std::unique_ptr<nix::Response>
ServiceClient::call(const std::string& server_address,
					const std::string& service,
					const std::string& route,
					const nix::Message& msg,
					size_t timeout_ms)
{
	return this->Client::call(
		server_address, service, route, msg, timeout_ms);
}

// proxy
std::unique_ptr<nix::Response>
ServiceClient::call(const std::string& server_address,
					const std::string& service,
					const std::string& route,
					size_t timeout_ms)
{
	return this->Client::call(
		server_address, service, route, timeout_ms);
}

bool ServiceClient::send_one_way(const std::string& route,
								 Message& msg)
{
	msg.set_meta("api_key", api_key_);

	return this->Client::send_one_way(
		server_address_, service_, route, msg);
}

// proxy:
bool ServiceClient::send_one_way(const std::string& server_address,
								 const std::string& service,
								 const std::string& route,
								 const nix::Message& msg)
{
	return this->Client::send_one_way(
		server_address, service, route, msg);
}

} // core
} // nix
