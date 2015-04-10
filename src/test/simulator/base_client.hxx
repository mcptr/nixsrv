#ifndef TEST_SIMULATOR_BASE_CLIENT_HXX
#define TEST_SIMULATOR_BASE_CLIENT_HXX

#include <string>


namespace simulator {


class BaseClient
{
public:
	BaseClient() = delete;
	BaseClient(const std::string& address);
	virtual ~BaseClient() = default;

protected:
	const std::string server_address_;
};


} // simulator

#endif
