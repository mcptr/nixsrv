#ifndef NIX_CORE_CLIENT_HXX
#define NIX_CORE_CLIENT_HXX

#include <string>
#include <atomic>
#include <mutex>
#include <yami4-cpp/yami.h>
#include <exception>

#include "nix/common.hxx"
#include "nix/message.hxx"
#include "nix/message/response.hxx"

#include <memory>


namespace nix {
namespace core {


class Client
{
public:
	Client(size_t max_timeout = 3000);
	//Client(const Client& other) = delete;
	virtual ~Client() = default;

	bool ping(const std::string& server_address, size_t timeout_ms = 1000);

	virtual
	std::unique_ptr<nix::Response>
	call(const std::string& server_address,
		 const std::string& service,
		 const std::string& route,
		 const nix::Message& msg = nix::Message(),
		 size_t timeout_ms = 0);

	virtual
	std::unique_ptr<nix::Response>
	call(const std::string& server_address,
		 const std::string& service,
		 const std::string& route,
		 size_t timeout_ms = 0);
	
	
	virtual bool send_one_way(const std::string& server_address,
							  const std::string& service,
							  const std::string& route,
							  const nix::Message& msg = nix::Message());

protected:
	yami::agent agent_;
	const size_t max_timeout_ms_;
};


} // core
} // nix

#endif
