#ifndef TEST_SIMULATOR_RESOLVER_CLIENT_HXX
#define TEST_SIMULATOR_RESOLVER_CLIENT_HXX

#include "base_client.hxx"

namespace simulator {


class ResolverClient : public BaseClient
{
public:
	ResolverClient() = delete;
	ResolverClient(const std::string& address)
		: BaseClient(address, "Resolver")
	{
	}

	void run(const std::string& thread_id = std::string());
};


} // simulator

#endif
