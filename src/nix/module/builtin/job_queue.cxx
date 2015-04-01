#include "job_queue.hxx"

namespace nix {
namespace module {

JobQueue::JobQueue(std::shared_ptr<ModuleAPI> api, size_t queue_size)
	: Module(api, "JobQueue", 1)
{
}

JobQueue::~JobQueue()
{
	for(auto& it : queues_) {
		api_->logger->log_debug(
			"Deleting queue: " + it.first 
			+ "(" + std::to_string(it.second->size()) + " jobs left)"
		);
		delete it.second;
	}
}

void JobQueue::init_queue(std::shared_ptr<nix::queue::InstanceConfig> inst)
{
	api_->logger->log_debug("JobQueue::init_queue(): " + inst->name +
							" / size " + std::to_string(inst->size));
	queues_.emplace(
		inst->name, 
		new Queue<Job>(inst->size)
	);

	Route::Handler_t handler = [](std::shared_ptr<IncomingMessage> im) {};

	std::string prefix = "job/" + inst->name;

	std::shared_ptr<Route> submit(
		new Route(prefix + "/submit", handler, Route::PUBLIC, Route::FUTURE)
	);

	std::shared_ptr<Route> progress_publish(
		new Route(prefix + "/progress/set", handler, Route::API_PRIVATE, Route::PUBLISH)
	);

	std::shared_ptr<Route> result_set(
		new Route(prefix + "/result/set", handler, Route::API_PRIVATE, Route::SYNC)
	);

	std::shared_ptr<Route> result_get(
		new Route(prefix + "/result/get", handler, Route::PUBLIC, Route::SYNC)
	);

	std::shared_ptr<Route> queue_clear(
		new Route(prefix + "/clear", handler, Route::ADMIN, Route::SYNC)
	);

	routes_.push_back(submit);
	routes_.push_back(progress_publish);
	routes_.push_back(result_set);
	routes_.push_back(result_get);
}


} // module
} // nix
