#ifndef NIX_CORE_SERVICE_CLIENT_HXX
#define NIX_CORE_SERVICE_CLIENT_HXX

#include <string>

#include "client.hxx"
#include "client_config.hxx"
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
				  const ClientConfig& config,
				  size_t max_timeout = 2000);

	virtual ~ServiceClient() = default;


	virtual bool ping_service(size_t timeout_ms = 1000);

	virtual std::unique_ptr<nix::Response>
	call(const std::string& route, size_t timeout_ms = 0);

	virtual std::unique_ptr<nix::Response>
	call(const std::string& route, Message& msg, size_t timeout_ms = 0);

	// proxy: need to not to hide overloaded
	std::unique_ptr<nix::Response>
	call(const std::string& server_address,
		 const std::string& service,
		 const std::string& route,
		 const nix::Message& msg = nix::Message(),
		 size_t timeout_ms = 0);

	// proxy: need to not to hide overloaded
	virtual
	std::unique_ptr<nix::Response>
	call(const std::string& server_address,
		 const std::string& service,
		 const std::string& route,
		 size_t timeout_ms = 0);

	virtual bool send_one_way(const std::string& route,
							  Message& msg);

	// proxy: need to not to hide overloaded
	virtual bool send_one_way(const std::string& server_address,
							  const std::string& service,
							  const std::string& route,
							  const nix::Message& msg = nix::Message());

protected:
	const std::string service_;
	const std::string server_address_;
	const ClientConfig client_config_;
};


} // core
} // nix

#endif
