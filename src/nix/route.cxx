#include "route.hxx"


namespace nix {


Route::Route(const std::string& route,
			 Handler_t handler,
			 AccessModifier_t am,
			 ProcessingType_t processing_type,
			 const std::string& description)
	: route_(route),
	  handler_(handler),
	  am_(am),
	  processing_type_(processing_type),
	  description_(description)
{
}

void Route::handle(std::shared_ptr<IncomingMessage> msg) const
{
	handler_(msg);
}

} // nix
