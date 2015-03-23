#ifndef NIX_UTIL_STRING_HXX
#define NIX_UTIL_STRING_HXX

#include <string>

namespace nix {
namespace util {
namespace string {

std::string& to_lower(std::string& s);
std::string& to_upper(std::string& s);
std::string& trim(std::string& s);

} // str
} // util
} // nix

#endif
