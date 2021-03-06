#ifndef NIX_EXCEPTION_HXX
#define NIX_EXCEPTION_HXX

#include <stdexcept>
#include <string>

namespace nix {

class FatalError :  public std::logic_error
{
public:
	FatalError() = delete;
	explicit FatalError(const std::string& m) : std::logic_error(m) { }
};

class RuntimeError :  public std::runtime_error
{
public:
	RuntimeError() = delete;
	explicit RuntimeError(const std::string& m) : std::runtime_error(m) { }
};

typedef FatalError InitializationError;

//typedef RuntimeError AuthError;

} // nix

#endif
