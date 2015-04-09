#ifndef NIX_SERVER_OPTIONS_HXX
#define NIX_SERVER_OPTIONS_HXX

#include <string>


namespace nix {
namespace server {


class Options
{
public:
	int start_time = 0;
	std::string nodename;
	std::string address = "tcp://*:*";

	bool tcp_nonblocking = true;
	int tcp_listen_backlog = 10;
	int dispatcher_threads = 1;

	bool development_mode = false;
};


} // server
} // nix

#endif
