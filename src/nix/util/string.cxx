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

void split(const std::string& str,
		   std::vector<std::string>& tokens,
		   const std::string& delimiters)
{
	using std::string;
	string::size_type last_pos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, last_pos);

    while(string::npos != pos || string::npos != last_pos) {
        tokens.push_back(str.substr(last_pos, pos - last_pos));
        last_pos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, last_pos);
    }
}


} // string
} // util
} // nix
