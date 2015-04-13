#ifndef NIX_INIT_COMMON_HXX
#define NIX_INIT_COMMON_HXX

#include <memory>
#include <string>

// util
#include "nix/util/fs.hxx"
#include "nix/util/pid.hxx"
#include "nix/util/string.hxx"

// nix
#include "nix/program_options.hxx"
#include "nix/module/manager.hxx"
#include "nix/object_pool.hxx"
#include "nix/db/connection.hxx"
#include "nix/db/options.hxx"
#include "nix/queue/options.hxx"
#include "nix/module/api.hxx"
#include "nix/module/manager.hxx"

// bulitin modules
#include "nix/module/builtin/debug.hxx"
#include "nix/module/builtin/service/job_queue.hxx"
#include "nix/module/builtin/service/resolver.hxx"
#include "nix/module/builtin/service/cache.hxx"

namespace nix {
namespace init {


using nix::ModuleManager;
using nix::ModuleAPI;
using nix::ObjectPool;
using nix::ProgramOptions;
using nix::db::Connection;
using nix::server::Options;


void setup_modules( const ProgramOptions& po,
					ModuleManager::Names_t& v);

void setup_server_options(const ProgramOptions& po,
						  nix::server::Options& options);

void setup_builtin_job_queue(const ProgramOptions& po,
							 std::shared_ptr<nix::module::JobQueue> jq);

void setup_db_pool(const ProgramOptions& po,
				   std::shared_ptr<ObjectPool<Connection>> pool);

void setup_logging(const ProgramOptions& options);

void setup_builtin_modules(const ProgramOptions& options,
						   std::shared_ptr<ModuleManager> module_manager,
						   std::shared_ptr<nix::ModuleAPI> mod_api,
						   nix::server::Options& server_options);


} // init
} // nix

#endif
