#include "route.hxx"


namespace nix {


Route::Route(const std::string& route,
			 Method_t method,
			 Handler_t& handler,
			 AccessModifier_t am,
			 const std::string& description)
	: route_(route),
	  method_(method),
	  handler_(handler),
	  am_(am),
	  description_(description)
{
}

void Route::operator()(const api::Request_t& req, api::Response& res)
{
	handler_(req, res);
}

// FIXME: implement theses
const std::string& Route::get_route() const {}
Method_t Route::get_method() const {}
AccessModifier_t& Route::get_access_modifier() const {}
const std::string& Route::get_description() const {}


} // nix
