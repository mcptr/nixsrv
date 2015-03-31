#ifndef NIX_SERVER_OPTIONS_HXX
#define NIX_SERVER_OPTIONS_HXX

#include <string>


namespace nix {
namespace server {

class Options
{
public:
	// server
	std::string address = "tcp://*:*";

	bool tcp_nonblocking = true;
	int tcp_listen_backlog = 10;
	int dispatcher_threads = 1;
};

} // server
} // nix

#endif
