#ifndef NIX_MODULE_BUILTIN_SERVICE_RESOLVER_HXX
#define NIX_MODULE_BUILTIN_SERVICE_RESOLVER_HXX

#include <memory>
#include <unordered_map>
#include "nix/module.hxx"
#include "nix/message/incoming.hxx"

namespace nix {
namespace module {


class Resolver : public Module
{
public:
	Resolver() = delete;
	virtual ~Resolver() = default;
	explicit Resolver(std::shared_ptr<ModuleAPI> api);

private:
	void bind(std::unique_ptr<IncomingMessage> msg);
	void resolve(std::unique_ptr<IncomingMessage> msg);
	void unbind(std::unique_ptr<IncomingMessage> msg);

	std::unordered_map<std::string, std::string> nodes_;
};


} // module
} // nix

#endif
