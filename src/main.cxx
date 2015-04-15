#include <iostream>
#include <ctime>
#include <memory>
#include <functional>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>

// os
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <csignal>
#include <errno.h>


// nix
#include "nix/common.hxx"
#include "nix/init/common.hxx"
#include "nix/init/daemon.hxx"
#include "nix/db/connection.hxx"
//#include "nix/db/options.hxx"
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
#include "nix/module/builtin/service/cache.hxx"


// util
#include "nix/util/fs.hxx"
#include "nix/util/pid.hxx"
#include "nix/util/string.hxx"

// devel
#include "nix/direct_handlers.hxx"

 
using nix::ModuleManager;
using nix::ModuleAPI;
using nix::ObjectPool;
using nix::ProgramOptions;
using nix::db::Connection;
using nix::server::Options;


void termination_signal_handler(int sig);

void setup_signals();
void block_and_wait();
void join_all_threads();


std::mutex main_mtx;
std::condition_variable main_cv;
std::atomic<bool> signaled { false };

INITIALIZE_EASYLOGGINGPP


int main(int argc, char** argv)
{
	srand(std::time(nullptr));

	ProgramOptions po;
	std::shared_ptr<ModuleManager> module_manager;
	std::shared_ptr<nix::Server> server;
	
	pid_t server_pid = 0;
	std::string server_pidfile;
	bool is_foreground = false;
	std::string base_dir;

	try {
		po.parse(argc, argv);
		if(po.has_help()) {
			po.display_help();
			return 0;
		}
		if(po.get<bool>("debug")) {
			po.dump_variables_map();
		}
	}
	catch(std::exception& e) {
		std::cerr << "Invalid command " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	is_foreground = po.get<bool>("foreground");
	base_dir = nix::util::fs::expand_user(
		po.get<std::string>("basedir"));

	nix::init::setup_logging(po);
	setup_signals();

	std::time_t result = std::time(nullptr);
	LOG(INFO) << "\n\n>>> Started: "
			  << std::asctime(std::localtime(&result)) << "\n";

	// server initialization
	try {

		ModuleManager::Names_t modules;
		nix::init::setup_modules(po, modules);

		std::shared_ptr<ObjectPool<Connection>> db_pool(
			new ObjectPool<Connection>()); // to be fixed

		nix::init::setup_db_pool(po, db_pool);

		Options server_options;
		nix::init::setup_server_options(po, server_options);

		if(!is_foreground) {
			server_pidfile = nix::util::fs::expand_user(
				po.get<std::string>("pidfile")
			);
			server_pid = nix::init::start_daemon(po, server_pidfile);
		}

		std::shared_ptr<ModuleAPI> mod_api(new ModuleAPI(db_pool));

		module_manager.reset(
			new ModuleManager(server_options, mod_api,
							  po.get<bool>("fatal")));

		module_manager->load(modules);

		nix::init::setup_builtin_modules(
			po, module_manager, mod_api, server_options);
		server.reset(new nix::Server(server_options));

		module_manager->register_routing(server);
		module_manager->start_all();
	}
	catch(const std::exception& e) {
		std::cerr << "std::exception (main()): " << e.what() << std::endl;
		LOG(ERROR) << e.what();
		return EXIT_FAILURE;
	}

	if(po.get<bool>("debug")) {
		server->register_object("echo", nix::direct_handlers::echo);
	}

	if(po.get<bool>("development-mode")) {
		LOG(WARNING) << "****** Starting in development mode "
				  << "(always allows KEY_TEST) ******";
	}

	try {
		server->start();
		module_manager->start_manager_thread();
		LOG(DEBUG) << "Running...";
	}
	catch(const std::exception& e) {
		LOG(ERROR) << e.what();
		return 1;
	}

	std::vector<std::thread> threads;
	threads.push_back(std::move(std::thread(block_and_wait)));

	for(auto&t : threads) {
		if(t.joinable()) {
			t.join();
		}
	}

	server->stop();
	module_manager.reset();

	nix::init::stop_daemon(server_pid, server_pidfile);
	return EXIT_SUCCESS;
}



void setup_signals()
{
	LOG(DEBUG) << "Setting signal handlers";
	std::signal(SIGINT, termination_signal_handler);
	std::signal(SIGTERM, termination_signal_handler);
}

void termination_signal_handler(int sig)
{
	LOG(DEBUG) << "Caught signal: " << sig;
	signaled = true;
	main_cv.notify_one();
}

void block_and_wait()
{
	std::unique_lock<std::mutex> lock(main_mtx);
	
	while(!signaled) {
		main_cv.wait(lock);
	}
	
}

// void crash_handler(int sig) {
// 	LOG(ERROR) << "Crashed";
// 	el::Helpers::logCrashReason(sig, true);
// 	el::Helpers::crashAbort(sig);
// }
