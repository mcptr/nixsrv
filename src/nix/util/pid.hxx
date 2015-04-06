#ifndef NIX_UTIL_PID_HXX
#define NIX_UTIL_PID_HXX

#include <sys/types.h>
#include <string>


namespace nix {
namespace util {
namespace pid {


bool pidfile_exists(const std::string& path);
bool pidfile_write(const std::string& path, pid_t pid);
bool pidfile_remove(const std::string& path);
pid_t pidfile_read(const std::string& path);
bool is_pid_alive(pid_t pid);


} // pid
} // util
} // nix

#endif
