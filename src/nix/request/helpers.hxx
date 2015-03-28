#ifndef NIX_REQUEST_HELPERS_HXX
#define NIX_REQUEST_HELPERS_HXX

#include "nix/route.hxx"

namespace nix {
namespace request {

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
