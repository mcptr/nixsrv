#include <memory>
#include <exception>
#include <thread>
#include <chrono>
#include <ctime>
#include <yami4-cpp/yami.h>

#include "nix/common.hxx"
#include "nix/message.hxx"

#include "worker.hxx"


namespace nix {
namespace module {
namespace debug {


Worker::Worker(std::shared_ptr<Queue<IncomingMessage>> q, int id)
	: q_(q),
	  worker_id_(id)
{
}

void Worker::run()
{
	std::unique_ptr<IncomingMessage> ptr;
	while(!q_->is_closed()) {
		q_->pop(std::move(ptr));
		if(ptr) {
			process(*ptr);
		}
	}
}

void Worker::process(IncomingMessage& msg)
{
	auto start = std::chrono::high_resolution_clock::now();

	//LOG(DEBUG) << "processing";
	int intval = msg.get("interval_ms", 0);
	if(intval) {
		LOG(DEBUG) << "sleep_for " << intval;
		std::this_thread::sleep_for(std::chrono::milliseconds(intval));
	}
	//nix::Message response("debug::Worker");
	//response.set("worker_id", worker_id_);

	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

	//response.set("processing_time_microseconds", microseconds);

	//LOG(DEBUG) << "reply";
	msg.reply();//response);
}

} // debug
} // module
} // nix
