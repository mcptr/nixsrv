#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <functional>

// for testing
#include <thread>
#include <chrono>
#include <vector>
#include <functional>
#include <sstream>
#include <mutex>
#include <condition_variable>

// daemon
#include <unistd.h> // linux
#include <cstdlib> // bsd
#include <csignal>

// nix
#include "nix/db/connection.hxx"
#include "nix/db/options.hxx"
#include "nix/logger.hxx"
#include "nix/module/api.hxx"
#include "nix/module/manager.hxx"
#include "nix/object_pool.hxx"
#include "nix/program_options.hxx"
#include "nix/server.hxx"
#include "nix/options.hxx"

// bulitin modules
#include "nix/module/builtin/job_queue.hxx"


// util
#include "nix/util/fs.hxx"
#include "nix/util/string.hxx"

// devel
#include "nix/direct_handlers.hxx"


using nix::Logger;
using nix::ModuleManager;
using nix::ModuleAPI;
using nix::ObjectPool;
using nix::ProgramOptions;
using nix::db::Connection;
using nix::server::Options;



void setup_modules(ModuleManager::Names_t& v,
				   const ProgramOptions& po);


void setup_server(Options& options,
				  const ProgramOptions& po);


void setup_db_pool(std::shared_ptr<ObjectPool<Connection>> pool,
				   const ProgramOptions& po);

//void inject_server_handlers(std::shared_ptr<nix::Server> server);

void serve(std::shared_ptr<nix::Server> server);

void termination_signal_handler(int sig);


std::mutex main_mtx;
std::condition_variable main_cv;
int signaled = false;

std::vector<std::thread> threads;

int main(int argc, char** argv)
{
	srand(time(NULL));

	ProgramOptions program_options;
	std::shared_ptr<nix::Server> server;
	std::shared_ptr<ModuleManager> module_manager;

	try {
		program_options.parse(argc, argv);
		
		if(program_options.has_help()) {
			program_options.display_help();
			return 0;
		}

		if(program_options.get<bool>("debug")) {
			program_options.dump_variables_map();
		}

		//--------------------------------------------------------------
		std::shared_ptr<Logger> logger(new Logger(program_options));

		ModuleManager::Names_t modules;
		setup_modules(modules, program_options);

		std::shared_ptr<ObjectPool<Connection>> db_pool(
			new ObjectPool<Connection>()); // to be fixed

		setup_db_pool(db_pool, program_options);

		std::shared_ptr<ModuleAPI> mod_api(new ModuleAPI(db_pool, logger));

		module_manager.reset(
			new ModuleManager(mod_api,
							  logger,
							  program_options.get<bool>("fatal")));

		module_manager->load(modules);

		Options server_options;
		setup_server(server_options, program_options);

		server.reset(new nix::Server(server_options, logger));

		// server->register_io_error_handler(
		// 	[&logger](int err, const char* errmsg) -> void
		// 	{
		// 		logger.log_error(errmsg);
		// 	}
		// );

		std::shared_ptr<nix::module::JobQueue> job_queue_module(
			new nix::module::JobQueue(mod_api, 100)
		);

		module_manager->add_builtin(job_queue_module);

		module_manager->register_routing(server);

	}
	catch(std::exception& e) {
		std::cerr << "std::exception (main()): " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	if(program_options.get<bool>("debug")) {
		//server->register_object("echo", nix::direct_handlers::echo);
	}

	std::signal(SIGINT, termination_signal_handler);
	std::signal(SIGTERM, termination_signal_handler);

	if(!program_options.get<bool>("foreground")) {
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

	std::cout << std::endl;

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

	std::string plugins_dir = 
		fs::resolve_path(po.get<std::string>("pluginsdir"));

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
	options.threads = po.get<int>("threads");

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

void termination_signal_handler(int /* sig */)
{
	signaled = true;
	main_cv.notify_all();
}
