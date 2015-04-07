#ifndef NIX_AUTH_HXX
#define NIX_AUTH_HXX

#include <unordered_map>

#include "nix/message/incoming.hxx"
#include "nix/route.hxx"


namespace nix {
namespace core {


class Auth
{
public:
	typedef enum {
		KEY_PUBLIC = 1,
		KEY_PRIVATE,
		KEY_ADMIN,
		KEY_TEST
	} KeyType_t;

	Auth(bool development_mode = false);
	bool check_access(const nix::IncomingMessage& msg,
					  const Route& route,
					  std::string& error_msg
	);

protected:
	// FIXME: for development only
	std::unordered_map<std::string, KeyType_t> auth_keys_;
	const bool development_mode_ = false;

};


} // core
} // nix

#endif
