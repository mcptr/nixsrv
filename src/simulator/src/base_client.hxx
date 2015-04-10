#ifndef TEST_SIMULATOR_BASE_CLIENT_HXX
#define TEST_SIMULATOR_BASE_CLIENT_HXX

#include <string>
#include <atomic>
#include <mutex>
#include <yami4-cpp/yami.h>

#include <thread>
#include <chrono>
#include <exception>
#include <stdexcept>

#include <nix/common.hxx>
#include <nix/message/incoming.hxx>


namespace simulator {

static int client_counter_ = 0;


class BaseClient
{
public:
	BaseClient() = delete;

	BaseClient(const std::string& address,
			   const std::string& module);

	virtual ~BaseClient() = default;

	virtual void run(const std::string& thread_id = std::string()) = 0;


	virtual void call(const std::string& route,
					  nix::Message& msg,
					  int timeout = -1) final;

	virtual void send_one_way(const std::string& route,
							  nix::Message& msg) final;

	virtual void on_replied(const nix::Message& m);
	virtual void on_rejected(const std::string& reason);
	virtual void on_abanoned();

protected:
	const int client_id_;
	const std::string server_address_;
	const std::string module_;
	yami::agent agent_;

	std::mutex mtx_;

	const std::string api_key_ = "_development_key_";
	const int max_timeout = 250;
};


} // simulator

#endif
