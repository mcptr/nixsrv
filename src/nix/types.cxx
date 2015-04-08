#include "types.hxx"


namespace nix {


std::string str_processing_type(Route::ProcessingType_t v)
{
	std::string value;

	switch(v) {
	case Route::VOID:
		value = "VOID";
		break;
	case Route::SYNC:
		value = "SYNC";
		break;
	case Route::ASYNC:
		value = "ASYNC";
		break;
	case Route::FUTURE:
		value = "FUTURE";
		break;
	case Route::PUBLISH:
		value = "PUBLISH";
		break;
	}

	return value;
}

std::string str_access_modifier(Route::AccessModifier_t v)
{
	std::string value;

	switch(v) {
	case Route::ANY:
		value = "ANY";
		break;
	case Route::PUBLIC:
		value = "PUBLIC";
		break;
	case Route::API_PRIVATE:
		value = "API_PRIVATE";
		break;
	case Route::ADMIN:
		value = "ADMIN";
		break;
	}

	return value;
}


} // nix
