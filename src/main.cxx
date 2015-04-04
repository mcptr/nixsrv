#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <functional>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

// os
#include <sys/stat.h>
#include <sys/types.h>


// daemon
#include <unistd.h> // linux
#include <cstdlib> // bsd
#include <csignal>


// nix
#include "nix/common.hxx"
#include "nix/db/connection.hxx"
#include "nix/db/options.hxx"
#include "nix/queue/options.hxx"
#include "nix/module/api.hxx"
#include "nix/module/manager.hxx"
#include "nix/object_pool.hxx"
#include "nix/program_options.hxx"
#include "nix/server.hxx"
#include "nix/options.hxx"

// bulitin modules
#include "nix/module/builtin/debug.hxx"
#include "nix/module/builtin/service/job_queue.hxx"
#include "nix/module/builtin/service/resolver.hxx"


// util
#include "nix/util/fs.hxx"
#include "nix/util/string.hxx"

// devel
#include "nix/direct_handlers.hxx"

 
using nix::ModuleManager;
using nix::ModuleAPI;
using nix::ObjectPool;
using nix::ProgramOptions;
using nix::db::Connection;
using nix::server::Options;


std::string get_log_file_path(const ProgramOptions& options);


void setup_modules(ModuleManager::Names_t& v,
				   const ProgramOptions& po);


void setup_server(Options& options,
				  const ProgramOptions& po);


void setup_db_pool(std::shared_ptr<ObjectPool<Connection>> pool,
				   const ProgramOptions& po);

//void inject_server_handlers(std::shared_ptr<nix::Server> server);

void serve(std::shared_ptr<nix::Server> server);

void termination_signal_handler(int sig);

void setup_builtin_job_queue(std::shared_ptr<nix::module::JobQueue> jq,
							 const ProgramOptions& po);


std::mutex main_mtx;
std::condition_variable main_cv;
int signaled = false;

std::vector<std::thread> threads;

INITIALIZE_EASYLOGGINGPP

void crash_handler(int sig) {
	LOG(ERROR) << "Crashed";
	el::Helpers::logCrashReason(sig, true);
	el::Helpers::crashAbort(sig);
}


int main(int argc, char** argv)
{
	srand(time(NULL));

	ProgramOptions po;
	std::shared_ptr<nix::Server> server;
	std::shared_ptr<ModuleManager> module_manager;
	
	try {
		po.parse(argc, argv);
		
		if(po.has_help()) {
			po.display_help();
			return 0;
		}

		if(po.get<bool>("debug")) {
			po.dump_variables_map();
		}

		//--------------------------------------------------------------
		bool is_foreground = po.get<bool>("foreground");

		std::string base_dir(
			nix::util::fs::resolve_path(po.get<std::string>("basedir"))
		);

		std::string logger_config_path = base_dir + "/etc/log.conf";
		el::Configurations conf(logger_config_path);

		conf.setGlobally(
			el::ConfigurationType::ToStandardOutput,
			is_foreground ? "true" : "false");

		conf.setGlobally(
			el::ConfigurationType::Filename,
			get_log_file_path(po));

		el::Loggers::reconfigureAllLoggers(conf);
		el::Helpers::setCrashHandler(crash_handler);

		// logging configuration end

		ModuleManager::Names_t modules;
		setup_modules(modules, po);

		std::shared_ptr<ObjectPool<Connection>> db_pool(
			new ObjectPool<Connection>()); // to be fixed

		setup_db_pool(db_pool, po);

		std::shared_ptr<ModuleAPI> mod_api(new ModuleAPI(db_pool));

		module_manager.reset(
			new ModuleManager(mod_api,
							  po.get<bool>("fatal")));

		module_manager->load(modules);

		Options server_options;
		setup_server(server_options, po);

		server.reset(new nix::Server(server_options));

		// MODULE: debug
		if(po.get<bool>("enable-debug")) {
			std::shared_ptr<nix::module::Debug> debug_module(
				new nix::module::Debug(mod_api)
			);
			module_manager->add_builtin(debug_module);
		}

		// MODULE: resolver
		if(po.get<bool>("enable-resolver")) {
			std::shared_ptr<nix::module::Resolver> resolver_module(
				new nix::module::Resolver(mod_api)
			);
			module_manager->add_builtin(resolver_module);
		}

		// MODULE: job queue
		if(po.get<bool>("enable-job-queue")) {
			std::shared_ptr<nix::module::JobQueue> job_queue_module(
				new nix::module::JobQueue(mod_api, 100)
			);
			setup_builtin_job_queue(job_queue_module, po);
			module_manager->add_builtin(job_queue_module);
		}

		module_manager->register_routing(server);
		module_manager->start_all();

	}
	catch(std::exception& e) {
		std::cerr << "std::exception (main()): " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	if(po.get<bool>("debug")) {
		server->register_object("echo", nix::direct_handlers::echo);
	}

	std::signal(SIGINT, termination_signal_handler);
	std::signal(SIGTERM, termination_signal_handler);

	bool is_foreground = po.get<bool>("foreground");
	if(!is_foreground) {
		int attached = daemon(1, 1);
		if(attached) {
			std::cerr << "daemon() failed: " << std::endl;
			return EXIT_FAILURE;
		}
	}

	threads.push_back(std::thread(serve, server));
	for(auto& th : threads) {
		th.join();
	}

	// serve(server);
	// int dummy;
	// std::cin >> dummy;

	if(!is_foreground) {
		std::cout << std::endl;
	}

	return EXIT_SUCCESS;
}


// ---------------------------------------------------------------------
// initialization helpers
// ---------------------------------------------------------------------

void setup_modules(ModuleManager::Names_t& v,
				   const nix::ProgramOptions& po)
{
	using namespace nix::util;

	std::string base_dir(
		fs::resolve_path(po.get<std::string>("basedir"))
	);

	std::string modules_dir = 
		fs::resolve_path(po.get<std::string>("modulesdir"));

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

void setup_server(Options& options,
					 const ProgramOptions& po)
{
	using std::string;
	using namespace nix;
	using nix::server::Options;

	options.address = po.get<string>("address");

	options.tcp_nonblocking = po.get<bool>("SERVER.tcp_nonblocking");
	options.tcp_listen_backlog = po.get<int>("SERVER.tcp_listen_backlog");
	options.dispatcher_threads = po.get<int>("SERVER.dispatcher_threads");
}

void setup_db_pool(std::shared_ptr<ObjectPool<Connection>> pool,
				   const ProgramOptions& po)
{
	using namespace std;

	string base_dir(
		nix::util::fs::resolve_path(po.get<string>("basedir"))
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

void serve(std::shared_ptr<nix::Server> server)
{
	std::unique_lock<std::mutex> lock(main_mtx);

	while(!signaled) {
		server->start();
		main_cv.wait(lock);
	}

	server->stop();
	server.reset();
	lock.unlock();

}

void setup_builtin_job_queue(std::shared_ptr<nix::module::JobQueue> jq,
							 const ProgramOptions& po)
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

std::string get_log_file_path(const ProgramOptions& po)
{
	std::string log_dir(
		nix::util::fs::resolve_path(
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
	
	return log_path;
}

void termination_signal_handler(int /* sig */)
{
	signaled = true;
	main_cv.notify_all();
}
