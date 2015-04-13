#include "job_queue_client.hxx"


namespace nix {
namespace core {


JobQueueClient::JobQueueClient(const std::string& server_address,
							   const std::string& api_key,
							   size_t max_timeout_ms_)
	: ServiceClient("JobQueue", server_address, api_key, max_timeout_ms_)
{
}

std::unique_ptr<nix::Response> JobQueueClient::submit(Message& job_data)
{
	// 2DO: use cases: failover, queue does not exist
	return this->call("job/submit", job_data);
}

std::unique_ptr<nix::Response>
JobQueueClient::get_work(const std::string& module)
{
	Message m;
	m.set("module", module);
	return this->call("job/work/get", m);
}

void JobQueueClient::set_progress(const std::string& queue_node,
								  const std::string& job_id,
								  const double progress)
{
	Message m;
	m.set("@job_id", job_id);
	m.set("progress", progress);
	std::string resolved_node = queue_node; // FIXME
	this->Client::send_one_way(
		resolved_node, "JobQueue", "job/progress/set", m);
}

void JobQueueClient::set_result(const std::string& queue_node,
								const std::string& job_id,
								Message& m)
{
	m.set("@job_id", job_id);
	std::string resolved_node = queue_node; // FIXME
	this->Client::call(
		resolved_node, "JobQueue", "job/result/set", m);
}

std::unique_ptr<nix::Response>
JobQueueClient::get_result(const std::string& queue_node,
						   const std::string& job_id)
{
	Message m;
	m.set("@job_id", job_id);
	std::string resolved_node = queue_node; // FIXME
	return this->Client::call(
		resolved_node, "JobQueue", "job/result/get", m);
}

std::unique_ptr<nix::Response>
JobQueueClient::status(const std::string& queue_node)
{
	Message empty;
	std::string resolved_node = queue_node; // FIXME
	return this->Client::call(
		resolved_node, "JobQueue", "status", empty);
}


} // core
} // nix


