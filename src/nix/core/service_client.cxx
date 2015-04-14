#include "service_client.hxx"


namespace nix {
namespace core {


ServiceClient::ServiceClient(const std::string& service,
							 const std::string& server_address,
							 const std::string& api_key,
							 size_t max_timeout)
	: service_(service),
	  server_address_(server_address),
	  api_key_(api_key),
	  max_timeout_(max_timeout > 0 ? max_timeout : 2000)
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
		timeout_ms > max_timeout_ ? max_timeout_ : timeout_ms);
}

bool ServiceClient::send_one_way(const std::string& route,
								 Message& msg)
{
	msg.set_meta("api_key", api_key_);

	bool success = false;
	try {
		this->Client::send_one_way(
			server_address_, service_, route, msg);
		success = true;
	}
	catch(const std::exception& e) {
		LOG(ERROR) << "send_one_way failed: " << e.what();
	}

	return success;
}


} // core
} // nix
