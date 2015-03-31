#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>

#include "job.hxx"


namespace nix {


Job::Job()
{
	std::stringstream s;
	s << boost::uuids::random_generator()();
	Job(s.str());
}

Job::Job(const std::string& id)
	: id_(id)
{
}


} // nix
