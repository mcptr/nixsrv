#include "server.hxx"
#include "base.hxx"

#include <unistd.h>

#include <sys/types.h>
#include <stdexcept>
#include <random>
#include <iostream>

#include <nix/util/fs.hxx>
#include <nix/util/pid.hxx>
#include <nix/core/client.hxx>


namespace test {


Server::Server(const std::string& address)
	: TestDaemon(),
	  address_(address)
{
	char* pr = getenv("PROJECT_ROOT");
	if(pr == nullptr) {
		throw std::runtime_error("PROJECT_ROOT env var not set");
	}

	std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<> distribution(65450, 65500);

	port_ = distribution(engine);

	project_root_.assign(nix::util::fs::resolve_path(pr));
	pidpath_ = "/tmp/nix-test-" + std::to_string(getpid()) + ".pid";
}

Server::Server(const std::vector<std::string>& modules,
			   const std::string& address)
	: Server(address)
{
	modules_ = modules;
}

void Server::set_arguments(std::vector<std::string>& args)
{
	std::string prog = project_root_ + "/bin/NIX";

	args.push_back(prog);
	args.push_back("-A");
	args.push_back(get_address());
	args.push_back("--nodename");
	args.push_back(nodename_);
	args.push_back("-c");
	args.push_back(project_root_ + "/etc/devel.ini");
	args.push_back("-F");
	//args.push_back("-D");
	//args.push_back("-v");
	args.push_back("--threads");
	args.push_back(std::to_string(2));
	args.push_back("--pidfile");
	args.push_back(pidpath_);
	args.push_back("--development-mode");

	for(auto& it : modules_) {
		args.push_back("--enable-" + it);
	}
}

pid_t Server::get_pid() const
{
	return nix::util::pid::pidfile_read(pidpath_);
}

bool Server::is_ready() const
{
	nix::core::Client client;
	size_t wait_loops = 10;
	while(wait_loops) {
		try {
			wait_loops--;
			if(client.call(get_address(), "ping", "", 500)) {
				return true;
			}
		}
		catch(const std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return false;
}

std::string Server::get_address() const
{
	std::string addr = address_ + ":" + std::to_string(port_);
	return addr;
}

} // test
