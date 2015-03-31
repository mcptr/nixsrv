#ifndef NIX_AUTH_HXX
#define NIX_AUTH_HXX

#include "nix/route.hxx"

namespace nix {


class Auth
{
public:
	std::string create_session(const std::string& user,
							   const std::string& passwd);

	std::string auth(const std::string& session_key,
					 const Route& route);

};


} // request
} // nix

#endif
