#include "worker.hxx"
#include "nix/common.hxx"
#include "nix/core/job_queue_client.hxx"


namespace nix {


QueueWorker::QueueWorker(const std::string& module,
						 const std::shared_ptr<nix::ModuleAPI> api)
	: module_(module),
	  api_(api)
{
}

void QueueWorker::run()
{
	while(true) { // FIXME
		auto client =  api_->client_pool->job_queue();
		if(client) {
			auto job = client->get_work(module_);
			if(job) {
				this->process_job(std::move(job));
			}
		}
		else {
			break;
		}
	}
}

void QueueWorker::set_result(std::unique_ptr<Job> job)
{
	auto client =  api_->client_pool->job_queue();
	if(client) {
		client->set_result(*job);
	}
	else {
		LOG(ERROR) << "Cannot set result. Failed to acquire job queue client";
	}
}


} // nix
