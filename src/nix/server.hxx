#ifndef NIX_SERVER_HXX
#define NIX_SERVER_HXX

#include <yami4-cpp/yami.h>
#include <string>
#include <memory>
#include <functional>
#include "nix/module.hxx"

#include "server/options.hxx"
#include "server/dispatcher.hxx"


namespace nix {

void is_node_alive_handler(yami::incoming_message& msg);


class Server {
public:
	typedef std::function<void(yami::incoming_message&)> DirectHandler_t;

	Server() = delete;
	Server(const Server& other) = delete;
	explicit Server(const server::Options& options);
	virtual ~Server();

	void start();
	void stop();

	void register_module(std::shared_ptr<const Module> inst);

	virtual	void register_object(const std::string& name,
								 DirectHandler_t handler);

	void default_route_handler(yami::incoming_message& msg);
private:
	const std::string address_;
	std::unique_ptr<yami::agent> agent_;
	std::string resolved_address_;
	std::unique_ptr<server::Dispatcher> dispatcher_;

	bool is_running_ = false;
};


} // nix

#endif

