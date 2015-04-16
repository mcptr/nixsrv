#ifndef NIX_CORE_JOB_QUEUE_CLIENT_HXX
#define NIX_CORE_JOB_QUEUE_CLIENT_HXX

#include <string>
#include <memory>
#include "service_client.hxx"
#include "nix/message/response.hxx"


namespace nix {
namespace core {


class JobQueueClient : public ServiceClient
{
public:
	JobQueueClient() = delete;

	JobQueueClient(const ClientConfig& config,
				   size_t max_timeout_ms_ = 2000);

	virtual ~JobQueueClient() = default;


	std::unique_ptr<nix::Response> submit(Message& job_data);

	std::unique_ptr<nix::Response> get_work(const std::string& module);

	void set_progress(const std::string& queue_node,
					  const std::string& job_id,
					  const double progress);

	void set_result(const std::string& queue_node,
					const std::string& job_id,
					Message& m);

	std::unique_ptr<nix::Response>
	get_result(const std::string& queue_node,
			   const std::string& job_id);

	std::unique_ptr<nix::Response>
	status(const std::string& queue_node);
};


} // core
} // nix

#endif
