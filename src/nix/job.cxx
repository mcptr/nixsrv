#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>
#include <ctime>
#include <cmath>

#include "job.hxx"


namespace nix {


Job::Job(const std::string& action,
		 const std::string& serialized_parameters)
	: action_(action),
	  parameters_(serialized_parameters)
{
	static boost::uuids::random_generator rg;

	std::stringstream s;
	s << rg();
	id_ = s.str();

	std::time_t tm = std::time(nullptr);
	ctime_ = std::localtime(&tm)->tm_sec;
}

void Job::set_progress(double progress)
{
	progress_ = progress;
}

double Job::get_progress() const
{
	return progress_;
}

const std::string& Job::get_action() const
{
	return action_;
}


} // nix
