#include "common.hxx"

// ---------------------------------------------------------------------
// initialization helpers
// ---------------------------------------------------------------------


namespace nix {
namespace init {


void setup_modules(const nix::ProgramOptions& po,
				   ModuleManager::Names_t& v)
{
	using namespace nix::util;

	std::string base_dir(
		fs::expand_user(po.get<std::string>("basedir"))
	);

	std::string modules_dir = 
		fs::expand_user(po.get<std::string>("modulesdir"));

	std::ifstream modules_config(base_dir + "/etc/modules.load");
	char line[256];
	while(modules_config.getline(line, 255)) {
		std::string module_name(line);
		string::trim(module_name);
		if(module_name.length() && module_name[0] != '#') {
			v.push_back(module_name);
		}
	}
	modules_config.close();
}


void setup_server_options(const ProgramOptions& po,
						  nix::server::Options& options)
{
	using std::string;
	using namespace nix;
	using nix::server::Options;

	options.start_time = std::time(nullptr);

	options.nodename = po.get<std::string>("nodename");
	options.address = po.get<string>("address");

	options.tcp_nonblocking = po.get<bool>("server.tcp_nonblocking");
	options.tcp_listen_backlog = po.get<int>("server.tcp_listen_backlog");

	options.dispatcher_threads = po.get<int>("server.dispatcher_threads");
	if(po.get<int>("threads")) {
		options.dispatcher_threads = po.get<int>("threads");
	}

	// development options
	options.development_mode = po.get<bool>("development-mode");
	options.enable_random_sleep = po.get<bool>("enable-random-sleep");

	// module manager
	options.manager_thread_enabled =
		po.get<bool>("module-manager.manager_thread_enabled");
	options.manager_thread_run_interval =
		po.get<int>("module-manager.manager_thread_run_interval");
	options.manager_thread_sleep_interval_ms =
		po.get<int>("module-manager.manager_thread_sleep_interval_ms");

	// builtins.cache
	options.cache_cleaner_enabled =
		po.get<bool>("builtins.cache.cleaner_enabled");
	options.cache_cleaner_run_interval =
		po.get<int>("builtins.cache.cleaner_run_interval");
	options.cache_cleaner_sleep_interval_ms =
		po.get<int>("builtins.cache.cleaner_sleep_interval_ms");

	// builtins.resolver
	options.resolver_monitor_enabled =
		po.get<bool>("builtins.resolver.monitor_enabled");
	options.resolver_monitor_run_interval =
		po.get<int>("builtins.resolver.monitor_run_interval");
	options.resolver_monitor_sleep_interval_ms =
		po.get<int>("builtins.resolver.monitor_sleep_interval_ms");
	options.resolver_monitor_max_failures =
		po.get<int>("builtins.resolver.monitor_max_failures");
	options.resolver_monitor_response_timeout_ms =
		po.get<int>("builtins.resolver.monitor_response_timeout_ms");

}

void setup_builtin_job_queue(const ProgramOptions& po,
							 std::shared_ptr<nix::module::JobQueue> jq)
{
	using namespace std;
	std::string config_path(po.get<string>("config"));
	if(po.is_verbose()) {
		std::cout << "Reading queues configuration " << std::endl;
	}

	nix::queue::Options options;
	options.parse(config_path);
	for(auto& inst : options.get_instances()) {
		jq->init_queue(inst);
	}
}

void setup_db_pool(const ProgramOptions& po,
				   std::shared_ptr<ObjectPool<Connection>> pool)
{
	using namespace std;

	string base_dir(
		nix::util::fs::expand_user(po.get<string>("basedir"))
	);

	std::string config_path(po.get<string>("dbconfig"));
	if(po.is_verbose()) {
		std::cout << "Reading db configuration: "
				  <<  config_path
				  << std::endl;
	}

	nix::db::Options options;
	options.parse(config_path);
	for(auto& inst : options.get_instances()) {
		// create connection
		// pool.insert
	}
}

void setup_logging(const ProgramOptions& po)
{
	std::string base_dir = nix::util::fs::expand_user(
		po.get<std::string>("basedir"));

	std::string log_dir(
		nix::util::fs::expand_user(
			po.get<std::string>("logdir")
		)
	);
	
	if(!nix::util::fs::path_exists(log_dir)) {
		mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
		if(mkdir(log_dir.c_str(), mode) != 0) {
			throw nix::InitializationError(
				"Cannot create log directory: " + log_dir);
		}

		if(!po.get<bool>("foreground")) {
			std::cout << "Created log directory: "
					  << log_dir << std::endl;
		}
	}

	std::string log_path = log_dir + "/error.log";

	// test if we can open the error.log for writing
	std::ofstream fh;
	fh.open(log_path, std::ios::out | std::ios::app | std::ios::binary);
	if(!fh.is_open()) {
		throw nix::InitializationError(
			"Cannot open log file: " + log_path);
	}

	//--------------------------------------------------------------
	std::string logger_config_path = base_dir + "/etc/log.conf";
	el::Configurations conf(logger_config_path);
	
	bool is_foreground = po.get<bool>("foreground");
	conf.setGlobally(
		el::ConfigurationType::ToStandardOutput,
		is_foreground ? "true" : "false");
	
	conf.setGlobally(el::ConfigurationType::Filename, log_path);
	
	el::Loggers::setDefaultConfigurations(conf, true);
	el::Loggers::reconfigureAllLoggers(conf);
	//el::Helpers::setCrashHandler(crash_handler);
	
}

void setup_builtin_modules(const ProgramOptions& po,
						   std::shared_ptr<ModuleManager> module_manager,
						   std::shared_ptr<nix::ModuleAPI> mod_api,
						   nix::server::Options& server_options)
{
	// MODULE: debug
	if(po.get<bool>("enable-debug")) {
		std::shared_ptr<nix::module::Debug> debug_module(
			new nix::module::Debug(mod_api, server_options)
		);
		module_manager->add_builtin(debug_module);
	}

	// MODULE: resolver
	if(po.get<bool>("enable-resolver")) {
		std::shared_ptr<nix::module::Resolver> resolver_module(
			new nix::module::Resolver(mod_api, server_options)
		);
		module_manager->add_builtin(resolver_module);
	}

	// MODULE: job queue
	if(po.get<bool>("enable-job-queue")) {
		std::shared_ptr<nix::module::JobQueue> job_queue_module(
			new nix::module::JobQueue(mod_api, server_options)
		);
		nix::init::setup_builtin_job_queue(po, job_queue_module);
		module_manager->add_builtin(job_queue_module);
	}

	// MODULE: cache
	if(po.get<bool>("enable-cache")) {
		std::shared_ptr<nix::module::Cache> cache_module(
			new nix::module::Cache(mod_api, server_options)
		);
		module_manager->add_builtin(cache_module);
	}
}


} // init
} // nix
