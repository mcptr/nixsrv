#include <unistd.h>
#include <fstream>
#include <csignal>

#include "pid.hxx"
#include "fs.hxx"

namespace nix {
namespace util {
namespace pid {

bool pidfile_exists(const std::string& path)
{
	return fs::file_exists(path);
}

bool pidfile_write(const std::string& path, pid_t pid)
{
	std::string pidpath(fs::expand_user(path));
	std::ofstream f(pidpath.c_str(), std::ios::trunc);
	if(f.is_open()) {
		f << pid;
		f.close();
	}
	return true;
}

pid_t pidfile_read(const std::string& path)
{
	pid_t pid = 0;
	std::string pidpath(fs::expand_user(path));
	if(pidfile_exists(pidpath)) {
		std::ifstream f(path.c_str());
		if(f.is_open()) {
			f >> pid;
			f.close();
		}
	}
	return pid;
}

bool pidfile_remove(const std::string& path)
{
	std::string pidpath(fs::expand_user(path));
	return (unlink(pidpath.c_str()) == 0);
}

bool is_pid_alive(pid_t pid)
{
	return (kill(pid, 0) == 0);
}


} // pid
} // util
} // nix
