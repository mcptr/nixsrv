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

	std::shared_ptr<Route> queue_manage(
		new Route("manage",
				  std::bind(&JobQueue::manage, this, _1),
				  Route::ADMIN,
				  Route::SYNC
		)
	);

	std::shared_ptr<Route> queue_status(
		new Route("status",
				  std::bind(&JobQueue::status, this, _1),
				  Route::API_PRIVATE,
				  Route::SYNC
		)
	);

	routes_.push_back(submit);
	routes_.push_back(work_get);
	routes_.push_back(progress_publish);
	routes_.push_back(result_set);
	routes_.push_back(result_get);
	routes_.push_back(queue_manage);
	routes_.push_back(queue_status);
}

JobQueue::~JobQueue()
{
	for(auto const& it : queues_) {
		LOG(DEBUG) << "Deleting queue: " << it.first 
				   << "("
				   << std::to_string(it.second->size())
				   << " jobs left)";

		it.second->clear();
		while(workers_awaiting_[it.first]->size()) {
			std::unique_ptr<IncomingMessage> msg;
			workers_awaiting_[it.first]->pop(std::move(msg));
			try {
				msg->reject(
					nix::service_unavailable,
					"Service unavailable."
				);
			}
			catch(...) {

			}
		}
		delete it.second;
	}
}

bool JobQueue::init_queue(std::shared_ptr<nix::queue::InstanceConfig> inst)
{
	return init_queue(inst->name, inst->size);
}

bool JobQueue::init_queue(const std::string& name, size_t size)
{
	size = size ? size : default_queue_size_;

	LOG(DEBUG) << name << " / size " << size;

	if(queues_.count(name)) {
		return false;
	}

	std::unique_lock<std::mutex> lock(mtx_);
	queues_.emplace(name, new Queue<Job>(size));
	workers_awaiting_.emplace(name, new Queue<IncomingMessage>(size));
	lock.unlock();
	return true;
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
			std::string action = msg->get("action", "");
			bool has_parameters = msg->exists("parameters");

			if(!(dest_queue.length() && action.length() && has_parameters)) {
				msg->fail(nix::data_invalid_content);
			}
			else {
				std::unique_ptr<Job> job(
					new Job(*msg));
				std::string job_id = job->get_id();

				bool passed_to_worker = false;
				size_t workers_count = 
					workers_awaiting_[it->first]->size();
				if(workers_count) {
					try {
						for(size_t count = 0;
							!passed_to_worker && count < workers_count;
							count++)
						{
							std::unique_ptr<IncomingMessage> worker_msg;
							workers_awaiting_[it->first]->pop(
								std::move(worker_msg));
							
							Message task;
							task.parse(job->to_string());
							worker_msg->reply(task);
							passed_to_worker = true;
						}
					}
					catch(const std::exception& e) {
						LOG(ERROR) << e.what();
					}
				}

				if(!passed_to_worker) {
					bool success;
					it->second->push(std::move(job), success);
					if(!success) {
						msg->fail(nix::temp_limit_reached, "Queue full");
					}
					else {
						// 2DO: store job in db 
						// (if queue is configured to be persistent),
						// will use a container now
						msg->clear();
						msg->set_meta("job_id", job_id);
						msg->reply(*msg);
					}
				}
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
				Message task;
				task.parse(ptr->to_string());
				msg->reply(task);

				std::unique_lock<std::mutex> lock(mtx_);
				in_progress_[ptr->get_id()] = std::move(ptr);
				lock.unlock();
			}
			else {
				bool success;
				workers_awaiting_[it->first]->push(std::move(msg), success);
			}
		}
	}
}

void JobQueue::set_progress(std::unique_ptr<IncomingMessage> msg)
{
	std::string job_id = msg->get_meta("job_id", std::string());

	auto it = in_progress_.find(job_id);
	if(it != in_progress_.end()) {
		std::unique_lock<std::mutex> lock(mtx_);
		if(msg->get("progress", 0.0)) {
			it->second->set_progress(msg->get("progress", 0.0));
		}
		lock.unlock();
	}
}


void JobQueue::set_result(std::unique_ptr<IncomingMessage> msg)
{
	std::string job_id = msg->get_meta("job_id", std::string());
	if(!job_id.length()) {
		msg->fail(nix::null_value);
	}
	else {
		if(!persistent_) {
			std::unique_lock<std::mutex> lock(mtx_);
			in_progress_.erase(job_id);
			std::unique_ptr<Job> result(new Job(*msg));
			result->set_progress(100);
			completed_[job_id] = std::move(result);
			completed_jobs_++;
			lock.unlock();
			msg->clear();
			msg->reply();
		}
	}
}

void JobQueue::get_result(std::unique_ptr<IncomingMessage> msg)
{
	std::string api_key = msg->get_meta("api_key", std::string());
	std::string job_id = msg->get_meta("job_id", std::string());
	msg->clear();

	if(!persistent_) {
		auto c_it = completed_.find(job_id);
		bool is_completed = (c_it != completed_.end());
		if(is_completed) {
			std::string submitter = c_it->second->get_api_key();
			if(submitter.compare(api_key) == 0) {
				std::unique_lock<std::mutex> lock(mtx_);
				msg->reply(*(c_it->second));
				completed_.erase(c_it);
				lock.unlock();
			}
			else {
				msg->fail(nix::access_denied);
			}
		}
		else {
			auto it = in_progress_.find(job_id);
			if(it != in_progress_.end()) {
				std::unique_lock<std::mutex> lock(mtx_);
				msg->reply(*(it->second), nix::not_ready);
				lock.unlock();
			}
			else {
				LOG(DEBUG) << " NOT STARTED ";
				msg->fail("Job not found or not yet started");
			}
		}
	}
}


/*
 * queue - string
 * clear - (single) true/false
 * clear_all - (ALL) true/false
 * enable - (single) true/false
 * enable_all - (ALL) true/false
 * remove - true/false
 * remove_all - true/false
 * create - { create : { size : x } }
 */
void JobQueue::manage(std::unique_ptr<IncomingMessage> msg)
{
	bool any_action_matched = false;
	// clear all (true/false)
	bool is_clear_all = msg->get("clear_all", false);
	if(is_clear_all) {
		clear_all();
		any_action_matched = true;
	}

	// remove all (true/false)
	bool is_remove_all = msg->get("remove_all", false);
	if(is_remove_all) {
		remove_all();
		any_action_matched = true;
	}

	// switch all on/off (true/false)
	bool has_switch = !msg->is_null("switch_all");
	bool state = msg->get("switch_all", true);
	if(has_switch) {
		switch_all(state);
		any_action_matched = true;
	}

	// operations on single queue
	const std::string name = msg->get("queue", "");
	auto it = queues_.find(name);
	if(!name.empty() && !msg->is_null("create")) {
		if(init_queue(name, msg->get("create.size", 0))) {
			msg->clear();
			msg->reply();
		}
		else {
			msg->fail("Failed to create queue. Already exists?");
		}
		any_action_matched = true;
	}
	else if(it != queues_.end()) {
		std::unique_lock<std::mutex> lock(mtx_);
		
		if(!msg->is_null("enable")) {
			it->second->set_enabled(msg->get("enable", true));
		}
		
		if(!msg->is_null("clear") && msg->get("clear", false)) {
			it->second->clear();
		}
		
		if(!msg->is_null("remove") && msg->get("remove", false)) {
			it->second->set_enabled(false);
			it->second->clear();
			queues_.erase(it);
		}
		
		lock.unlock();
		msg->reply();
		any_action_matched = true;
	}

	if(!any_action_matched) {
		msg->fail(nix::null_value);
	}
}

void JobQueue::status(std::unique_ptr<IncomingMessage> msg)
{
	msg->clear();

	msg->set("total_completed", completed_jobs_);

	std::unique_lock<std::mutex> lock(mtx_);

	long long total_in_progress = in_progress_.size();
	long long total_results_awaiting = completed_.size();
	msg->set("in_progress", total_in_progress);
	msg->set("results_awaiting", total_results_awaiting);
	long long total_pending = 0;
	for(auto it : queues_) {
		const std::string prefix = "queues." + it.first;
		long long pending = it.second->size();
		total_pending += pending;
		msg->set(prefix + ".closed", it.second->is_closed());
		msg->set(prefix + ".pending", pending);
	}
	msg->set("total_pending", total_pending);

	lock.unlock();
	msg->reply(*msg);
}

void JobQueue::clear_all()
{
	// each queue is takes care of thread safety
	for(auto it : queues_) {
		it.second->clear();
	}
}

void JobQueue::switch_all(bool state)
{
	for(auto it : queues_) {
		it.second->set_enabled(state);
	}
}

void JobQueue::remove_all()
{
	std::unique_lock<std::mutex> lock(mtx_);
	switch_all(false);
	clear_all();
	queues_.clear();
	lock.unlock();
}


} // module
} // nix
