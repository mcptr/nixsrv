#ifndef NIX_MODULE_BUILTIN_DEBUG_HXX
#define NIX_MODULE_BUILTIN_DEBUG_HXX

#include <memory>
#include <yami4-cpp/yami.h>
#include "nix/module.hxx"


namespace nix {
namespace module {


class Debug : public Module
{
public:
	Debug() = delete;
	virtual ~Debug() = default;

	explicit Debug(std::shared_ptr<ModuleAPI> api);
	void debug_sync(std::shared_ptr<IncomingMessage> msg);
	void debug_async(std::shared_ptr<IncomingMessage> msg);

private:
	size_t replies_ = 0;
};


} // module
} // nix


#endif
