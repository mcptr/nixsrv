#ifndef NIX_NET_TRANSPORT_OPTIONS_HXX
#define NIX_NET_TRANSPORT_OPTIONS_HXX

#include <string>


namespace nix {
namespace core {
namespace net {
namespace transport {

class Options
{
public:
	typedef enum { UNIX, TCP, UDP, INPROC } AddressFamily_t;

	// server
	AddressFamily_t address_family = TCP;
	std::string listen_address = "127.0.0.1";
	size_t port = 9876;
	int threads = 1;

	// transport
	bool tcp_nonblocking = true;
	int tcp_listen_backlog = 10;
	int dispatcher_threads = 1;
};

} // transport
} // net
} // core
} // nix

#endif
