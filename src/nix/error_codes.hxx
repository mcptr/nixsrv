#ifndef NIX_ERROR_CODES_HXX
#define NIX_ERROR_CODES_HXX

namespace nix {
namespace error_code {


// 1000 (auth failures)
const int auth_unauthorized = 1001;

// 2000 (temporary problem)
const int temp_limit_reached = 2001;


} // error_code
} // nix

#endif
