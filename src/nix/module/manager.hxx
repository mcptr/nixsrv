#ifndef NIX_MODULE_MANAGER_HXX
#define NIX_MODULE_MANAGER_HXX

#include <vector>
#include <string>
#include <memory>

#include "nix/core/module.hxx"
#include "nix/core/db/connection.hxx"
#include "nix/core/logger.hxx"
#include "nix/core/object_pool.hxx"
#include "nix/core/net/transport.hxx"

#include "api.hxx"
#include "instance.hxx"


namespace nix {
namespace core {

class ModuleManager
{
public:
	typedef std::vector<std::string> Names_t;


	ModuleManager() = delete;
	ModuleManager(ModuleAPI& api,
				  Logger& logger,
				  bool fatal = false);

	void load(const Names_t& modules);
	void load(const std::string& module_path);
	void unload();

	void register_routing(std::shared_ptr<Transport> t);
private:
	ObjectPool<ModuleInstance> modules_pool_;

	ModuleAPI& api_;
	Logger& logger_;
	bool fatal_;
};

} // core
} // nix

#endif
