#ifndef NIX_MODULE_BUILTIN_HXX
#define NIX_MODULE_BUILTIN_HXX

#include "nix/module.hxx"
#include "nix/server/options.hxx"

namespace nix {

class BuiltinModule : public Module
{
public:
	BuiltinModule() = delete;
	BuiltinModule(std::shared_ptr<ModuleAPI> api,
				  const std::string& name,
				  const int version,
				  const nix::server::Options& options)
		: Module(api, name, version),
		  options_(options)
	{
	}

	virtual ~BuiltinModule() = default;

protected:
	const nix::server::Options options_;
};


} // nix

#endif

