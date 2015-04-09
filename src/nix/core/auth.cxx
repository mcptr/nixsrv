#include "auth.hxx"


namespace nix {
namespace core {


Auth::Auth(bool development_mode)
	: development_mode_(development_mode)
{
	// insert keys used by unit tests
	if(development_mode) {
		// always allowed in development_mode
		auth_keys_["_development_key_"] = KEY_TEST;
		// specific levels in development_mode
		auth_keys_["_development_key_public_"] = KEY_PUBLIC;
		auth_keys_["_development_key_private_"] = KEY_PRIVATE;
		auth_keys_["_development_key_admin_"] = KEY_ADMIN;
	}
}

bool Auth::check_access(const nix::IncomingMessage& msg,
						const Route& route,
						std::string& error_msg)

{
	if(route.get_access_modifier() == Route::ANY) {
		return true;
	}

	std::string api_key = msg.get("@api_key", "");

	// enable test api key for unit tests.
	// this requires development_mode to be set.
	// this mode must be explicitely given on server
	// startup
	if(development_mode_
	   && auth_keys_[api_key] == KEY_TEST) {
		LOG(WARNING) << "\nDevelopment mode: ALLOWING "
					 << api_key << "\n";
		return true;
	}
	
	// end of development code


	if(api_key.empty()) {
		error_msg = "No api_key found";
		return false;
	}

	size_t is_key_known = auth_keys_.count(api_key);
	if(!is_key_known) {
		error_msg = "Invalid api_key " + api_key;
		return false;
	}

	bool authorized = false;
	switch(route.get_access_modifier()) {
	case Route::PUBLIC:
		// this key is known - public allows an known key
		authorized = true;
		break;
	case Route::API_PRIVATE:
		authorized = (
			(auth_keys_[api_key] == KEY_PRIVATE) || 
			(auth_keys_[api_key] == KEY_ADMIN)
		);
		break;
	case Route::ADMIN:
		authorized = (auth_keys_[api_key] == KEY_ADMIN);
		break;
	default:
		authorized = false;
		error_msg = "Unauthorized";
	}

	return authorized;
}


} // core
} // nix
