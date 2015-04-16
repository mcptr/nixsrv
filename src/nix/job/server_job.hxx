#ifndef NIX_JOB_SERVER_JOB_HXX
#define NIX_JOB_SERVER_JOB_HXX

#include "nix/job.hxx"


namespace nix {


class ServerJob : public Job
{
public:
	ServerJob() = default;
	explicit ServerJob(const nix::Message& msg);

	void set_progress(double progress);
};


} // nix

#endif

