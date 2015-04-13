#include "server.hxx"
#include "base.hxx"

#include <sys/types.h>
#include <unistd.h>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <random>
#include <cstring>
#include <errno.h>
#include <cstdlib>
#include <signal.h>
#include <iostream>

#include <nix/util/fs.hxx>
#include <nix/util/pid.hxx>


namespace test {


Server::Server(const std::string& address)
	: address_(address)
{
	char* pr = getenv("PROJECT_ROOT");
	if(pr == nullptr) {
		throw std::runtime_error("PROJECT_ROOT env var not set");
	}

	project_root_.assign(nix::util::fs::resolve_path(pr));
	pidpath_ = "/tmp/nix-test-" + std::to_string(getpid()) + ".pid";
}

Server::Server(const std::vector<std::string>& modules,
			   const std::string& address)
	: Server(address)
{
	modules_ = modules;
}

bool Server::start()
{
	std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<> distribution(65450, 65500);

	port_ = distribution(engine);

	std::string prog = project_root_ + "/bin/NIX";

	std::string listen_address = address_ + ":" + std::to_string(port_);
	std::vector<std::string> args;
	args.push_back(prog);
	args.push_back("-A");
	args.push_back(listen_address);
	args.push_back("-c");
	args.push_back(project_root_ + "/etc/devel.ini");
	args.push_back("-D");
	args.push_back("-F");
	args.push_back("-v");
	args.push_back("--threads");
	args.push_back(std::to_string(2));
	args.push_back("--pidfile");
	args.push_back(pidpath_);
	args.push_back("--development-mode");

	for(auto& it : modules_) {
		args.push_back("--enable-" + it);
	}

	char** arguments = new char* [args.size() + 1];

	for(size_t i = 0; i < args.size(); i++) {
		arguments[i] = (char*) args[i].c_str();
	}
	arguments[args.size()] = (char*)NULL;
	pid_t pid = fork();

	if(pid > 0) {
		if(execv(prog.c_str(), arguments) == -1) {
			std::cerr << "execv() failed: " << strerror(errno) << std::endl;
			return false;
		}
	}
	else if(pid == 0) {
		// 2DO: get a client, and ping until it becomes ready
		return true;
	}

	std::cerr << "fork() failed: " << strerror(errno) << std::endl;
	return false;
}

void Server::stop()
{
	pid_t pid = nix::util::pid::pidfile_read(pidpath_);
	if(pid) {
		int status = kill(pid, SIGTERM);
		if(status == -1) {
			std::cerr << "kill(TERM) failed: "
					  << strerror(errno) << std::endl;
		}
		int wait_loops = 20;
		while(kill(pid, 0) == 0 && wait_loops) {
			usleep(500 * 1000);
			wait_loops--;
		}

		if(kill(pid, 0) == 0) {
			std::cerr << "Resorting to SIGKILL..." << std::endl;
			kill(pid, SIGKILL);
		}
	}
}


} // test
