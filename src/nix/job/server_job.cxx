#include "server_job.hxx"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>

namespace nix {


ServerJob::ServerJob(const nix::Message& msg)
	: Job(msg)
{
	// if !job_id yet - this is a new incoming task,
	// and we need to generate new id
	if(!id_.length()) {
		static boost::uuids::random_generator rg;
		std::stringstream s;
		s << rg();
		id_ = s.str();

		ctime_ = std::time(nullptr);
	}
}

void ServerJob::set_progress(double progress)
{
	if(progress >= 100.0) {
		completed_ = true;
		progress_ = 100.0;
	}
	else {
		progress_ = progress;
	}
}


} // nix
