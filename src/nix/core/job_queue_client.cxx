#include "job_queue_client.hxx"


namespace nix {
namespace core {


JobQueueClient::JobQueueClient(const ClientConfig& config,
							   size_t max_timeout_ms_)
	: ServiceClient(
		"JobQueue",
		config.srv_job_queue_address,
		config,
		max_timeout_ms_),
	  resolver_client_(new ResolverClient(config))
{
}

std::unique_ptr<nix::Response> JobQueueClient::submit(Job& job)
{
	// 2DO: use cases: failover, queue does not exist
	job.set_api_key(client_config_.api_key_private);
	job.set_origin_node(client_config_.local_nodename);
	return this->call_service("job/submit", job);
}

std::unique_ptr<nix::Job>
JobQueueClient::get_work(const std::string& module)
{
	Message m;
	m.set_meta("api_key", client_config_.api_key_private);
	m.set("module", module);

	auto response = this->call_service("job/work/get", m);
	if(response->is_status_ok()) {
		std::unique_ptr<nix::Job> job(
			new Job(*response));
		return job;
	}
	return nullptr;
}

bool JobQueueClient::set_progress(const Job& job)
{
	std::string resolved_address =
		resolver_client_->resolve_node(job.get_origin_node());

	if(resolved_address.length()) {
		this->Client::send_one_way(
			resolved_address, "JobQueue", "job/progress/set", job);
		return true;
	}
	return false;
}

bool JobQueueClient::set_result(const Job& job)
{
	std::string resolved_address =
		resolver_client_->resolve_node(job.get_origin_node());
	if(resolved_address.length()) {
		this->Client::call(
			resolved_address, "JobQueue", "job/result/set", job);
		return true;
	}
	return false;
}

std::unique_ptr<nix::Job>
JobQueueClient::get_result(const Job& job)
{
	std::string resolved_address =
		resolver_client_->resolve_node(job.get_origin_node());
	if(resolved_address.length()) {
		auto response = this->Client::call(
			resolved_address, "JobQueue", "job/result/get", job);
		if(response->is_replied()) {
			std::unique_ptr<nix::Job> result_job(
				new Job(*response));
			return result_job;
		}
		else {
			return nullptr;
		}
	}
	return nullptr;
}

std::unique_ptr<nix::Response>
JobQueueClient::status(const std::string& queue_node)
{
	std::string resolved_address;
	if(!queue_node.empty()) {
		resolved_address =
			resolver_client_->resolve_node(queue_node);
	}
	else {
		resolved_address = server_address_;
	}

	Message empty;
	if(resolved_address.length()) {
		return this->Client::call(
			resolved_address, "JobQueue", "status", empty);
	}
	return nullptr;
}


} // core
} // nix
