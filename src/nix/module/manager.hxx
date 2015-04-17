#ifndef NIX_MODULE_MANAGER_HXX
#define NIX_MODULE_MANAGER_HXX

#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <thread>

#include "nix/module.hxx"
#include "nix/db/connection.hxx"
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
	ModuleManager(const nix::server::Options& server_options,
				  std::shared_ptr<ModuleAPI> api,
				  bool fatal = false);


	virtual ~ModuleManager();

	void load(const Names_t& modules);
	void load(const std::string& module_path);
	void unload();

	void add_builtin(std::shared_ptr<Module> module);

	void register_routing(std::shared_ptr<nix::Server> server);

	void start_all();
	void stop_all();

	//void bind(const std::string& resolver_address);

	void start_manager_thread();
private:
	const nix::server::Options& server_options_;
	std::shared_ptr<ModuleAPI> api_;
	bool fatal_;

	ObjectPool<ModuleInstance> modules_pool_;
	std::vector<std::shared_ptr<Module>> builtins_;

	bool running_ = false;

	std::mutex mtx_;


	std::atomic<bool> manager_stop_flag_ {false};
	std::timed_mutex manager_mtx_;
	std::thread manager_thread_;

	void manager();

	// notify resolver service that modules are present
	void notify_resolver_bind();
	// notify resolver service our modules go away
	void notify_resolver_unbind();
};


} // nix

#endif
