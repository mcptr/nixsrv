#include <algorithm>
#include <boost/algorithm/string.hpp>
#include "string.hxx"

namespace nix {
namespace util {
namespace string {

std::string& to_lower(std::string& s)
{
	std::transform(s.begin(),
				   s.end(),
				   s.begin(),
				   ::tolower
	);
	return s;
}

std::string& to_upper(std::string& s)
{
	std::transform(s.begin(),
				   s.end(),
				   s.begin(),
				   ::toupper
	);
	return s;
}

std::string& trim(std::string& s)
{
	boost::trim(s);
	return s;
}

} // str
} // util
} // nix
