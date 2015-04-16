#ifndef NIX_JOB_CLIENT_JOB_HXX
#define NIX_JOB_CLIENT_JOB_HXX

#include "nix/job.hxx"


namespace nix {


class ClientJob : public Job
{
public:
	ClientJob();
	ClientJob(const std::string& module, const std::string& action);
	explicit ClientJob(const nix::Message& msg);
	virtual ~ClientJob() = default;

	void set_module(const std::string& module);
	void set_action(const std::string& action);
	void set_api_key(const std::string& api_key);
};


} // nix

#endif
