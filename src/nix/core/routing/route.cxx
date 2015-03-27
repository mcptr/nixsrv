#include "route.hxx"

namespace nix {
namespace core {

Route::Route(const std::string& route,
			 Method_t method,
			 AccessModifier_t am,
			 const std::string& description)
{
}

// FIXME: implement theses
const std::string& Route::get_route() const {}
Method_t Route::get_method() const {}
AccessModifier_t& Route::get_access_modifier() const {}
const std::string& Route::get_description() const {}


} // core
} // nix
