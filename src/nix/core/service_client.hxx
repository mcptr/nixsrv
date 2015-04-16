#ifndef NIX_CORE_SERVICE_CLIENT_HXX
#define NIX_CORE_SERVICE_CLIENT_HXX

#include <memory>
#include <string>

#include "client.hxx"
#include "client_config.hxx"
#include "nix/common.hxx"
#include "nix/message.hxx"
#include "nix/message/response.hxx"


namespace nix {
namespace core {

// forward
class ClientPool;

class ServiceClient : public Client
{
public:
	ServiceClient() = delete;

	ServiceClient(const std::string& service,
				  const std::string& server_address,
				  const ClientConfig& config,
				  size_t max_timeout = 2000);

	virtual ~ServiceClient() = default;


	virtual bool ping_service(size_t timeout_ms = 1000);

	virtual std::unique_ptr<nix::Response>
	call_service(const std::string& route,
				 const std::string& msg = std::string(),
				 size_t timeout_ms = 0);

	virtual bool send(const std::string& route,
					  const std::string& msg = std::string());

protected:
	const std::string service_;
	const std::string server_address_;
	const ClientConfig client_config_;
};


} // core
} // nix

#endif
