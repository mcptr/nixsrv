#ifndef NIX_TYPES_HXX
#define NIX_TYPES_HXX

#include <string>
#include "route.hxx"


namespace nix {


std::string str_processing_type(Route::ProcessingType_t v);
std::string str_access_modifier(Route::AccessModifier_t v);


} // nix

#endif
