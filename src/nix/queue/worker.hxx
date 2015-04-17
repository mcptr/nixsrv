#ifndef NIX_QUEUE_WORKER_HXX
#define NIX_QUEUE_WORKER_HXX

#include <memory>
#include <string>
#include "nix/module/api.hxx"
#include "nix/job.hxx"


namespace nix {


class QueueWorker
{
public:
	QueueWorker() = delete;

	QueueWorker(
		const std::string& module,
		const std::shared_ptr<nix::ModuleAPI> api
	);

	virtual ~QueueWorker() = default;

	virtual void run() final;

protected:
	const std::string module_;
	const std::shared_ptr<nix::ModuleAPI> api_;

	virtual void set_result(std::unique_ptr<Job> job) final;
	virtual void process_job(std::unique_ptr<Job> job) = 0;
};


} // nix

#endif
