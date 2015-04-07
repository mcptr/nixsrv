#include "auth.hxx"


namespace nix {
namespace core {


Auth::Auth(bool development_mode)
{
	auth_keys_["example_key_public"] = KEY_PUBLIC;
	auth_keys_["example_key_private"] = KEY_PRIVATE;
	auth_keys_["example_key_admin"] = KEY_ADMIN;
	if(development_mode) {
		auth_keys_["_internal_key_test_"] = KEY_TEST;
	}
}

bool Auth::check_access(const nix::IncomingMessage& msg,
						const Route& route,
						std::string& error_msg)

{
	if(route.get_access_modifier() == Route::ANY) {
		return true;
	}

	std::string msg_key = msg.get("@api_key", "");
	if(msg_key.empty()) {
		error_msg = "No api_key found";
		return false;
	}

	size_t is_key_known = auth_keys_.count(msg_key);
	if(!is_key_known) {
		error_msg = "Invalid api_key";
		return false;
	}

	bool authorized = false;
	switch(route.get_access_modifier()) {
	case Route::PUBLIC:
		// this key is known - public allows an known key
		authorized = true;
		break;
	case Route::API_PRIVATE:
		authorized = (auth_keys_[msg_key] == KEY_PRIVATE);
		break;
	case Route::ADMIN:
		authorized = (auth_keys_[msg_key] == KEY_ADMIN);
		break;
	default:
		authorized = false;
		error_msg = "Unauthorized";
	}

	return authorized;
}


} // core
} // nix
