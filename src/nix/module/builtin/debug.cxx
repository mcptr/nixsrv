#include <functional>
#include <thread>
#include <chrono>

#include "nix/common.hxx"
#include "nix/route.hxx"
#include "nix/message/object.hxx"

#include "debug.hxx"


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

void Debug::start()
{
	LOG(DEBUG) << "Creating queue";
	q_.reset(new Queue<IncomingMessage>(10));
	for(int i = 0; i < 3; i++) {
		LOG(DEBUG) << "Creating worker";
		workers_.push_back(debug::Worker(q_, i));
	}

	for(auto& wrk : workers_) {
		LOG(DEBUG) << "Starting worker thread";
		threads_.push_back(
			std::move(std::thread(&debug::Worker::run, std::ref(wrk))));
	}
}

void Debug::stop()
{
	q_->close();
	LOG(DEBUG) << "Queue closed";
	int i = 0;
	for(auto& it : threads_) {
		if(it.joinable()) {
			LOG(DEBUG) << "joining thread " << i;
			it.join();
			i++;
		}
	}

	q_.reset();
}

void Debug::debug_sync(std::unique_ptr<IncomingMessage> msg)
{
	replies_++;
	OutgoingMessage out("Debug");
	msg->reply(out);
}

void Debug::debug_async(std::unique_ptr<IncomingMessage> msg)
{
	bool success;
	q_->push(std::move(msg), success);
	if(!success) {
		LOG(DEBUG) << "Queue full";
		nix::OutgoingMessage om("debug_async");
		om.set_error_code(nix::error_code::temp_limit_reached);
		om.set_error_msg("Temporary failure: request queue full");
		msg->reply(om);
	}
}


} // module
} // nix
