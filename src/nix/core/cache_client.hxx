#ifndef NIX_CORE_CACHE_CLIENT_HXX
#define NIX_CORE_CACHE_CLIENT_HXX

#include <string>
#include "service_client.hxx"


namespace nix {
namespace core {


class CacheClient : public ServiceClient
{
public:
	CacheClient() = delete;

	CacheClient(const std::string& server_address,
				const std::string& api_key,
				size_t max_timeout_ms_ = 2000);

	virtual ~CacheClient() = default;


	bool store(const std::string& key, const std::string& value);
	bool store(const std::string& key, const Message& value);

	bool retrieve(const std::string& key, Message& result);

	void remove(const std::string& key);

	bool status(Message& result);
};


} // core
} // nix

#endif
