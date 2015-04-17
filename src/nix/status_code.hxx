#ifndef NIX_STATUS_CODE_HXX
#define NIX_STATUS_CODE_HXX


namespace nix {


typedef enum
{
	null_value = -1,
	ok = 0,
	fail,
	not_ready,
	service_unavailable,
	temporary_failue,

	// access other than core::auth (e.g. api_key mismatch)
	access_denied = 51,

	// 10x (auth failures)
	auth_unauthorized = 101,

	// 20x (temporary problem)
	temp_limit_reached = 201,

	// 50x (data errors)
	data_invalid_content = 501
} StatusCode_t;


} // nix

#endif
