#include <functional>
#include <thread>
#include <chrono>

#include "debug.hxx"
#include "nix/route.hxx"
#include "nix/message/object.hxx"


namespace nix {
namespace module {


Debug::Debug(std::shared_ptr<ModuleAPI> api)
	: Module(api, "Debug", 1)
{
	using namespace std::placeholders;

	std::shared_ptr<Route> debug_sync_route(
		new Route("debug_sync", std::bind(&Debug::debug_sync, this, _1), Route::ANY, Route::SYNC)
	);

	std::shared_ptr<Route> debug_async_route(
		new Route("debug_async", std::bind(&Debug::debug_async, this, _1), Route::ANY, Route::FUTURE)
	);

	routes_.push_back(debug_sync_route);
	routes_.push_back(debug_async_route);
}

void Debug::debug_sync(std::shared_ptr<IncomingMessage> msg)
{
	replies_++;
	OutgoingMessage out("Debug");
	msg->reply(out);
}

void Debug::debug_async(std::shared_ptr<IncomingMessage> msg)
{
	std::thread t(
		[msg, this]() {
			this->api_->logger->log_debug("Debug::debug_async() thread");
			int intval = msg->get_integer("interval_ms", 1000);
			this->api_->logger->log_debug(
				"Debug::debug_async() going to sleep for (ms): " + std::to_string(intval)
			);
			std::this_thread::sleep_for(std::chrono::milliseconds(intval));
			OutgoingMessage out("Debug");
			nix::Object o;
			o.set("slept_for", intval);
			out.set_content(o);
			msg->reply(out);
			this->api_->logger->log_debug("Debug::debug_async() finished with reply");
		}
	);
	this->api_->logger->log_debug("Debug::debug_async() detaching");
	t.detach();
	this->api_->logger->log_debug("Debug::debug_async() callback exiting");
}


} // module
} // nix
