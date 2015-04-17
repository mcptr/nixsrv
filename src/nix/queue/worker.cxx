#include "worker.hxx"
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
		auto job = api_->client_pool->job_queue()->get_work(module_);
		if(job) {
			this->process_job(std::move(job));
		}
	}
}

void QueueWorker::set_result(std::unique_ptr<Job> job)
{
	api_->client_pool->job_queue()->set_result(*job);
}


} // nix


