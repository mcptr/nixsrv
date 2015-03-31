#include "job_queue.hxx"

namespace nix {
namespace module {

JobQueue::JobQueue(std::shared_ptr<ModuleAPI> api, size_t queue_size)
	: Module(api, "JobQueue", 1)
{
	Route::Handler_t handler = [](IncomingMessage& im) {};
	submitted_.reset(new Queue<Job>(queue_size));
	std::shared_ptr<Route> submit(
		new Route("job/submit", handler, Route::AUTH, Route::DEFERRED)
	);

	std::shared_ptr<Route> progress_publish(
		new Route("job/progress/set", handler, Route::AUTH, Route::PUBLISH)
	);

	std::shared_ptr<Route> result_set(
		new Route("job/result/set", handler, Route::AUTH, Route::SYNC)
	);

	std::shared_ptr<Route> result_get(
		new Route("job/result/get", handler, Route::AUTH, Route::SYNC)
	);

	routes_.push_back(submit);
	routes_.push_back(progress_publish);
	routes_.push_back(result_set);
	routes_.push_back(result_get);
}

} // module
} // nix
