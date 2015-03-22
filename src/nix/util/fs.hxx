#ifndef NIX_UTIL_FS_HXX
#define NIX_UTIL_FS_HXX

#include <string>

namespace nix {
namespace util {
namespace fs {

bool path_exists(const std::string& path);
bool file_exists(const std::string& path);
std::string resolve_path(const std::string& path);
std::string expand_user(const std::string& path);
std::string wexpand(const std::string& path);

} // fs
} // util
} // nix

#endif
