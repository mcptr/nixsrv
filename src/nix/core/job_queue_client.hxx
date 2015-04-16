#ifndef NIX_CORE_JOB_QUEUE_CLIENT_HXX
#define NIX_CORE_JOB_QUEUE_CLIENT_HXX

#include <memory>
#include <string>
#include "service_client.hxx"
#include "resolver_client.hxx"
#include "nix/message/response.hxx"
#include "nix/job/client_job.hxx"

namespace nix {
namespace core {


class JobQueueClient : public ServiceClient
{
public:
	JobQueueClient() = delete;

	JobQueueClient(const ClientConfig& config,
				   size_t max_timeout_ms_ = 2000);

	virtual ~JobQueueClient() = default;


	std::unique_ptr<nix::Response> submit(ClientJob& job_data);

	std::unique_ptr<nix::ClientJob> get_work(const std::string& module);

	bool set_progress(const std::string& queue_node,
					  const std::string& job_id,
					  const double progress);

	bool set_result(const std::string& queue_node,
					const std::string& job_id,
					Message& m);

	std::unique_ptr<nix::Response>
	get_result(const std::string& queue_node,
			   const std::string& job_id);

	std::unique_ptr<nix::Response>
	status(const std::string& queue_node);

protected:
	const std::unique_ptr<ResolverClient> resolver_client_;
};


} // core
} // nix

#endif
