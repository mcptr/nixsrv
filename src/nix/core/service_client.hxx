#ifndef NIX_CORE_SERVICE_CLIENT_HXX
#define NIX_CORE_SERVICE_CLIENT_HXX

#include <string>

#include "client.hxx"
#include "nix/common.hxx"
#include "nix/message.hxx"
#include "nix/message/response.hxx"


namespace nix {
namespace core {


class ServiceClient : public Client
{
public:
	ServiceClient() = delete;

	ServiceClient(const std::string& service,
				  const std::string& server_address,
				  const std::string& api_key,
				  size_t max_timeout = 0);

	virtual ~ServiceClient() = default;

	virtual
	std::unique_ptr<nix::Response> call(const std::string& route,
										Message& msg,
										size_t timeout_ms = 0);

	virtual bool send_one_way(const std::string& route,
							  Message& msg,
							  size_t timeout_ms = 0);

private:
	const std::string service_;
	const std::string server_address_;
	const std::string api_key_;
	const size_t max_timeout_ = 2000;
};


} // core
} // nix

#endif
