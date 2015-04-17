#ifndef NIX_CORE_CLIENT_HXX
#define NIX_CORE_CLIENT_HXX

#include <string>
#include <atomic>
#include <mutex>
#include <yami4-cpp/yami.h>
#include <exception>

#include "client_config.hxx"

#include "nix/common.hxx"
#include "nix/message.hxx"
#include "nix/message/response.hxx"

#include <memory>


namespace nix {
namespace core {


class Client
{
public:
	Client(const ClientConfig& config = ClientConfig(), size_t max_timeout = 3000);
	//Client(const Client& other) = delete;
	virtual ~Client() = default;

	bool ping(const std::string& server_address, size_t timeout_ms = 1000);

	virtual
	std::unique_ptr<nix::Response>
	call(const std::string& server_address,
		 const std::string& service,
		 const std::string& route,
		 const std::string& msg = std::string(),
		 size_t timeout_ms = 0) final;

	virtual bool send_one_way(const std::string& server_address,
							  const std::string& service,
							  const std::string& route,
							  const std::string& msg = std::string());

protected:
	const ClientConfig client_config_;
	const size_t max_timeout_ms_;

	yami::agent agent_;
};


} // core
} // nix

#endif
