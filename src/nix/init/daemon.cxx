#include "daemon.hxx"
#include "nix/exception.hxx"

#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


namespace nix {
namespace init {


pid_t start_daemon(const nix::ProgramOptions& po,
				   const std::string& pidfile_path)
{
	std::string dirname = nix::util::fs::dirname(pidfile_path);
	bool dir_exists = nix::util::fs::path_exists(dirname);

	if(!dir_exists) {
		int error = nix::util::fs::create_dir(dirname);
		if(error) {
			std::cerr << "Cannot write pid: " 
					  << pidfile_path 
					  << std::endl;
			throw InitializationError("start_daemon() failed");
		}
	}

	if(nix::util::fs::file_exists(pidfile_path)) {
		pid_t other_pid = nix::util::pid::pidfile_read(pidfile_path);
		if(nix::util::pid::is_pid_alive(other_pid)) {
			std::cerr << "Another instance already running: "
					  << other_pid << std::endl;
			throw InitializationError("Another instance running.");
		}
		else {
			std::cerr << "Removed stale pid file, old pid: "
					  << other_pid << std::endl;
		}
	}
	int daemonized = daemon(1, (po.get<bool>("no-close-fds") ? 1 : 0));
	if(daemonized == -1) {
		std::cerr << "daemon() failed" << std::endl;
		throw InitializationError("daemon() failed");
	}
	if(daemonized == 0) {
		daemonized = daemon(0, (po.get<bool>("no-close-fds") ? 1 : 0));
		if(daemonized == -1) {
			std::cerr << "second daemon() failed" << std::endl;
			throw InitializationError("second daemon() call failed");
		}
	}

	pid_t server_pid = getpid();
	nix::util::pid::pidfile_write(pidfile_path, server_pid);
	return server_pid;
}

void stop_daemon(pid_t server_pid, const std::string& pidfile_path)
{
	if(server_pid) {
		nix::util::pid::pidfile_remove(pidfile_path);
		exit(0);
	}
}


} // init
} // nix
