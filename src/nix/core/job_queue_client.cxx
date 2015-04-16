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

std::unique_ptr<nix::Response> JobQueueClient::submit(ClientJob& job)
{
	// 2DO: use cases: failover, queue does not exist
	job.set_api_key(client_config_.api_key_private);
	job.set_origin_node(client_config_.local_nodename);
	return this->call_service("job/submit", job);
}

std::unique_ptr<nix::ClientJob>
JobQueueClient::get_work(const std::string& module)
{
	Message m;
	m.set("module", module);
	auto response = this->call_service("job/work/get", m);
	if(response->is_status_ok()) {
		std::unique_ptr<nix::ClientJob> job(
			new ClientJob(*response));
	}
	return nullptr;
}

bool JobQueueClient::set_progress(const std::string& queue_node,
								  const std::string& job_id,
								  const double progress)
{
	Message m;
	m.set("@job_id", job_id);
	m.set("progress", progress);
	std::string resolved_address =
		resolver_client_->resolve_node(queue_node);
	if(resolved_address.length()) {
		this->Client::send_one_way(
			resolved_address, "JobQueue", "job/progress/set", m);
		return true;
	}
	return false;
}

bool JobQueueClient::set_result(const std::string& queue_node,
								const std::string& job_id,
								Message& m)
{
	m.set("@job_id", job_id);
	std::string resolved_address =
		resolver_client_->resolve_node(queue_node);
	if(resolved_address.length()) {
		this->Client::call(
			resolved_address, "JobQueue", "job/result/set", m);
		return true;
	}
	return false;
}

std::unique_ptr<nix::Response>
JobQueueClient::get_result(const std::string& queue_node,
						   const std::string& job_id)
{
	Message m;
	m.set("@job_id", job_id);
	std::string resolved_address =
		resolver_client_->resolve_node(queue_node);
	if(resolved_address.length()) {
		return this->Client::call(
			resolved_address, "JobQueue", "job/result/get", m);
	}
	return nullptr;
}

std::unique_ptr<nix::Response>
JobQueueClient::status(const std::string& queue_node)
{
	Message empty;
	std::string resolved_address =
		resolver_client_->resolve_node(queue_node);
	if(resolved_address.length()) {
		return this->Client::call(
			resolved_address, "JobQueue", "status", empty);
	}
	return nullptr;
}


} // core
} // nix
