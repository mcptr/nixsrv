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

void Route::handle(const impl::Request_t& req, Response& res) const
{
	handler_(req, res);
}

} // nix
