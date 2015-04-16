#include "client.hxx"
#include "nix/common.hxx"
#include <exception>


namespace nix {
namespace core {

Client::Client(const ClientConfig& config, size_t max_timeout_ms)
	: client_config_(config),
	  max_timeout_ms_(max_timeout_ms)
{
}

std::unique_ptr<nix::Response>
Client::call(const std::string& server_address,
			  const std::string& service,
			  const std::string& route,
			  const std::string& msg,
			  size_t timeout_ms)
{
	yami::parameters params;
	params.set_string("message", msg);

	std::unique_ptr<yami::outgoing_message> om;
	try {
		om = std::move(
			agent_.send(server_address, service, route, params));
	}
	catch(const std::exception& e) {
		return nullptr;
	}

	if(timeout_ms) {
		om->wait_for_completion(
			timeout_ms > max_timeout_ms_ ? max_timeout_ms_ : timeout_ms
		);
	}
	else {
		om->wait_for_completion(max_timeout_ms_);
	}

	std::unique_ptr<nix::Response> response(
		new nix::Response(std::move(om)));

	return response;
}

bool Client::ping(const std::string& server_address, size_t timeout_ms)
{
	auto response = call(server_address, "ping", "", "", timeout_ms);
	return response->is_replied();
}


bool Client::send_one_way(const std::string& server_address,
						  const std::string& service,
						  const std::string& route,
						  const std::string& msg)
{
	yami::parameters params;
	params.set_string("message", msg);

	bool success = false;
	try {
		agent_.send_one_way(server_address, service, route, params);
		success = true;
	}
	catch(const std::exception& e) {
		LOG(ERROR) << "send_one_way failed: " << e.what();
	}

	return success;
}


} // core
} // nix
