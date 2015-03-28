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

// implementation type (for compile time)
#include "nix/impl_types.hxx"

// nix
#include "nix/db/connection.hxx"
#include "nix/db/options.hxx"
#include "nix/logger.hxx"
#include "nix/module/api.hxx"
#include "nix/module/manager.hxx"
#include "nix/object_pool.hxx"
#include "nix/program_options.hxx"
#include "nix/transport.hxx"
#include "nix/transport/options.hxx"

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
using nix::Transport;
using nix::transport::Options;



void setup_modules(ModuleManager::Names_t& v,
				   const ProgramOptions& po);


void setup_transport(Options& options,
					 const ProgramOptions& po);


void setup_db_pool(ObjectPool<Connection>& pool,
				   const ProgramOptions& po);

void inject_transport_handlers(std::shared_ptr<nix::impl::Transport_t> transport);

void serve(std::shared_ptr<nix::impl::Transport_t> transport);

void termination_signal_handler(int sig);


std::mutex main_mtx;
std::condition_variable main_cv;
int signaled = false;

std::vector<std::thread> threads;

int main(int argc, char** argv)
{
	srand(time(NULL));

	ProgramOptions program_options;
	std::shared_ptr<nix::impl::Transport_t> transport;

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
		Logger logger(program_options);

		ModuleManager::Names_t modules;
		setup_modules(modules, program_options);

		ObjectPool<Connection> db_pool; // to be fixed
		setup_db_pool(db_pool, program_options);

		ModuleAPI mod_api(logger, db_pool);

		ModuleManager module_manager(
			mod_api,
			logger,
			program_options.get<bool>("fatal")
		);

		module_manager.load(modules);

		Options transport_options;
		setup_transport(transport_options, program_options);

		transport.reset(new nix::impl::Transport_t(transport_options));

		transport->register_io_error_handler(
			[&logger](int err, const char* errmsg) -> void
			{
				logger.log_error(errmsg);
			}
		);

		module_manager.register_routing(transport);

	}
	catch(std::exception& e) {
		std::cerr << "std::exception (main()): " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	if(program_options.get<bool>("debug")) {
		transport->register_object("echo", nix::direct_handlers::echo);
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

	threads.push_back(std::thread(serve, transport));
	for(auto& th : threads) {
		th.join();
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

void setup_transport(Options& options,
					 const ProgramOptions& po)
{
	using std::string;
	using namespace nix;
	using nix::transport::Options;

	options.listen_address = po.get<string>("listen_address");
	options.port = po.get<int>("port");
	options.threads = po.get<int>("threads");

	options.tcp_nonblocking = po.get<bool>("TRANSPORT-YAMI.tcp_nonblocking");
	options.tcp_listen_backlog = po.get<int>("TRANSPORT-YAMI.tcp_listen_backlog");
	options.dispatcher_threads = po.get<int>("TRANSPORT-YAMI.dispatcher_threads");

	string address_family(po.get<string>("address_family"));
	util::string::to_lower(util::string::trim(address_family));


	if(address_family.compare("unix") == 0) {
		options.address_family = Options::UNIX;
	}
	else if(address_family.compare("tcp") == 0) {
		options.address_family = Options::TCP;
	}
	else if(address_family.compare("udp") == 0) {
		options.address_family = Options::UDP;
	}
	else if(address_family.compare("inproc") == 0) {
		options.address_family = Options::INPROC;
	}
}

void setup_db_pool(ObjectPool<Connection>& pool,
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

void serve(std::shared_ptr<nix::impl::Transport_t> transport)
{
	std::unique_lock<std::mutex> lock(main_mtx);

	while(!signaled) {
		transport->start();
		main_cv.wait(lock);
	}

	transport->stop();
	transport.reset();
	lock.unlock();

}

void termination_signal_handler(int /* sig */)
{
	signaled = true;
	main_cv.notify_all();
}
