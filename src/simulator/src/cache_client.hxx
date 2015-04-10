#ifndef TEST_SIMULATOR_CACHE_CLIENT_HXX
#define TEST_SIMULATOR_CACHE_CLIENT_HXX

#include "base_client.hxx"


namespace simulator {


class CacheClient : public BaseClient
{
public:
	CacheClient() = delete;
	CacheClient(const std::string& address)
		: BaseClient(address, "Cache")
	{
	}

	void run(const std::string& thread_id = std::string());
	void on_replied(const nix::Message& m);

private:
	const std::string cache_key_ = "cache_key";
	const std::string cache_value_ = "cache_value";
};


} // simulator

#endif
