#include "nix/common.hxx"
#include "nix/job.hxx"
#include "nix/status_code.hxx"

#include "job_queue.hxx"

namespace nix {
namespace module {

JobQueue::JobQueue(std::shared_ptr<ModuleAPI> api,
				   const nix::server::Options& options)
	: BuiltinModule(api, "JobQueue", 1, options)
{
	Route::Handler_t handler = [](std::shared_ptr<IncomingMessage> im) {};

	std::shared_ptr<Route> submit(
		new Route("job/submit",
				  std::bind(&JobQueue::submit, this, _1),
				  Route::API_PRIVATE,
				  Route::FUTURE)
	);

	std::shared_ptr<Route> work_get(
		new Route("job/work/get",
				  std::bind(&JobQueue::get_work, this, _1),
				  Route::API_PRIVATE,
				  Route::SYNC)
	);

	std::shared_ptr<Route> progress_publish(
		new Route("job/progress/set",
				  std::bind(&JobQueue::set_progress, this, _1),
				  Route::API_PRIVATE,
				  Route::VOID
				  // FIXME: Route::PUBLISH
		)
	);

	std::shared_ptr<Route> result_set(
		new Route("job/result/set",
				  std::bind(&JobQueue::set_result, this, _1),
				  Route::API_PRIVATE,
				  Route::SYNC
		)
	);

	std::shared_ptr<Route> result_get(
		new Route("job/result/get",
				  std::bind(&JobQueue::get_result, this, _1),
				  Route::API_PRIVATE,
				  Route::SYNC
		)
	);

	std::shared_ptr<Route> queue_clear(
		new Route("job/clear", handler, Route::ADMIN, Route::SYNC)
	);

	routes_.push_back(submit);
	routes_.push_back(progress_publish);
	routes_.push_back(result_set);
	routes_.push_back(result_get);
	routes_.push_back(queue_clear);
}

JobQueue::~JobQueue()
{
	for(auto& it : queues_) {
		LOG(DEBUG) << "Deleting queue: " << it.first 
				   << "("
				   << std::to_string(it.second->size())
				   << " jobs left)";

		delete it.second;
	}
}

void JobQueue::init_queue(std::shared_ptr<nix::queue::InstanceConfig> inst)
{
	LOG(DEBUG) << inst->name
			   << " / size " << std::to_string(inst->size);

	queues_.emplace(inst->name, new Queue<Job>(inst->size));
}

void JobQueue::submit(std::unique_ptr<IncomingMessage> msg)
{
	std::string dest_queue = msg->get("module", "");

	if(!persistent_) {
		auto it = queues_.find(dest_queue);
		if(it == queues_.end()) {
			msg->fail("No queue for module: " + dest_queue);
		}
		else if(!it->second->is_closed()) {
			msg->remove("module");
			std::string action = msg->get("action", "");
			
			std::unique_ptr<Job> job(new Job(action, msg->to_string()));
			std::string job_id = job->get_id();
			
			bool success;
			it->second->push(std::move(job), success);
			if(!success) {
				msg->fail(nix::temp_limit_reached, "Queue full");
			}
			else {
				// 2DO: store job in db (if queue is configured to be persistent,
				// will use a container now
				msg->clear();
				msg->set("job_id", job_id);
				msg->reply();
			}
		}
		else {
			msg->fail("Queue closed");
		}
	}
}

void JobQueue::get_work(std::unique_ptr<IncomingMessage> msg)
{
	std::string queue_name = msg->get("module", "");

	if(!persistent_) {
		auto it = queues_.find(queue_name);
		if(it == queues_.end()) {
			msg->fail("No queue for module: " + queue_name);
		}
		else {
			std::unique_ptr<Job> ptr;
			it->second->pop(std::move(ptr));
			if(ptr) {
				Message work;
				work.set("action", ptr->get_action());
				work.set("payload", ptr->get_serialized_parameters());
				//work.set("@queue_address", nodename);
				msg->reply();
			}
		}
	}
}

void JobQueue::set_progress(std::unique_ptr<IncomingMessage> msg)
{
	std::string job_id = msg->get("job_id", "");

	auto it = in_progress_.find(job_id);
	if(it != in_progress_.end()) {
		it->second->set_progress(msg->get("progress", 0.0));
	}
}


void JobQueue::set_result(std::unique_ptr<IncomingMessage> msg)
{
	std::string job_id = msg->get("job_id", "");

	in_progress_.erase(job_id);
	if(!persistent_) {
		completed_[job_id] = std::move(msg);
	}
}

void JobQueue::get_result(std::unique_ptr<IncomingMessage> msg)
{
	std::string api_key = msg->get("@api_key", "");
	std::string job_id = msg->get("job_id", "");

	msg->clear();

	if(!persistent_) {
		auto c_it = completed_.find(job_id);
		bool is_completed = (c_it != completed_.end());
		if(is_completed) {
			std::string submitter = c_it->second->get("@api_key", "");
			if(submitter.compare(api_key) == 0) {
				msg->reply(*(c_it->second));
				completed_.erase(c_it);
			}
			else {
				msg->fail(nix::access_denied);
			}
		}
		else {
			auto it = in_progress_.find(job_id);
			if(it != in_progress_.end()) {
				msg->set("progress", it->second->get_progress());
				msg->reply(*msg, nix::not_ready);
			}
			else {
				msg->fail("Job not found or not yet started");
			}
		}
	}
}

// void JobQueue::get_progress(std::unique_ptr<IncomingMessage> msg)
// {
// 	std::string job_id = msg->get("job_id", "");

// 	auto it = in_progress_.find(job_id);
// 	if(it != in_progress_.end()) {
// 		it.second->set_progress(msg->get("progress", 0.0));
// 	}
// 	else {
// 		msg->fail(nix::null_value);
// 	}
// }


} // module
} // nix
