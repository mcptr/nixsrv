#ifndef NIX_CORE_JOB_QUEUE_CLIENT_HXX
#define NIX_CORE_JOB_QUEUE_CLIENT_HXX

#include <memory>
#include <string>
#include "service_client.hxx"
#include "resolver_client.hxx"
#include "nix/message/response.hxx"
#include "nix/job.hxx"

namespace nix {
namespace core {


class JobQueueClient : public ServiceClient
{
public:
	JobQueueClient() = delete;

	JobQueueClient(const ClientConfig& config,
				   size_t max_timeout_ms_ = 2000);

	virtual ~JobQueueClient() = default;


	std::unique_ptr<nix::Response> submit(Job& job_data);

	std::unique_ptr<nix::Job> get_work(const std::string& module);

	bool set_progress(const Job& job);

	bool set_result(const Job& job);

	std::unique_ptr<nix::Job>
	get_result(const Job& job);

	std::unique_ptr<nix::Response>
	status(const std::string& queue_node = std::string());

protected:
	const std::unique_ptr<ResolverClient> resolver_client_;
};


} // core
} // nix

#endif
