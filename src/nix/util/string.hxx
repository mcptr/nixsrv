#ifndef NIX_UTIL_STRING_HXX
#define NIX_UTIL_STRING_HXX

#include <string>
#include <vector>

namespace nix {
namespace util {
namespace string {

std::string& to_lower(std::string& s);
std::string& to_upper(std::string& s);
std::string& trim(std::string& s);

void split(const std::string& str,
		   std::vector<std::string>& tokens,
		   const std::string& delimiters = " ");

} // string
} // util
} // nix

#endif
