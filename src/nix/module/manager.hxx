#ifndef NIX_MODULE_MANAGER_HXX
#define NIX_MODULE_MANAGER_HXX

#include <vector>
#include <string>
#include <memory>

#include "nix/module.hxx"
#include "nix/db/connection.hxx"
#include "nix/logger.hxx"
#include "nix/object_pool.hxx"
#include "nix/server.hxx"

#include "api.hxx"
#include "instance.hxx"


namespace nix {


class ModuleManager
{
public:
	typedef std::vector<std::string> Names_t;


	ModuleManager() = delete;
	ModuleManager(std::shared_ptr<ModuleAPI> api,
				  std::shared_ptr<Logger> logger,
				  bool fatal = false);


	virtual ~ModuleManager();

	void load(const Names_t& modules);
	void load(const std::string& module_path);
	void unload();

	void add_builtin(std::shared_ptr<Module> module);

	void register_routing(std::shared_ptr<nix::Server> server);
private:
	ObjectPool<ModuleInstance> modules_pool_;
	std::vector<std::shared_ptr<Module>> builtins_;

	std::shared_ptr<ModuleAPI> api_;
	std::shared_ptr<Logger> logger_;
	bool fatal_;
};


} // nix

#endif
