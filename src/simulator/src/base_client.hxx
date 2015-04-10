#ifndef TEST_SIMULATOR_BASE_CLIENT_HXX
#define TEST_SIMULATOR_BASE_CLIENT_HXX

#include <string>
#include <atomic>
#include <yami4-cpp/yami.h>

#include <thread>
#include <chrono>

#include <nix/common.hxx>
#include <nix/message/incoming.hxx>


namespace simulator {


class BaseClient
{
public:
	BaseClient() = delete;

	BaseClient(const std::string& address);

	virtual ~BaseClient() = default;

	virtual void run() = 0;


	void call(const std::string& module,
			  const std::string& route,
			  const nix::Message& msg,
			  int timeout = -1);

	virtual void on_replied(nix::Message& m);
	virtual void on_rejected(const std::string& reason);
	virtual void on_abanoned();

protected:
	const std::string server_address_;
	yami::agent agent_;

	const std::string api_key_ = "_development_key_";
};


} // simulator

#endif
