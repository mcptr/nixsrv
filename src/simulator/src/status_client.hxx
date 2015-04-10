#ifndef TEST_SIMULATOR_STATUS_CLIENT_HXX
#define TEST_SIMULATOR_STATUS_CLIENT_HXX

#include "base_client.hxx"


namespace simulator {


class StatusClient : public BaseClient
{
public:
	StatusClient() = delete;
	StatusClient(const std::string& address,
				 bool verbose,
				 int interval_sec = 0)
		: BaseClient(address, "Status"),
		  verbose_(verbose),
		  intval_(interval_sec)
	{
	}

	void run(const std::string& thread_id = std::string());
	void on_replied(const nix::Message& m);
private:
	bool verbose_ = false;
	int intval_ = 0;
};


} // simulator

#endif
