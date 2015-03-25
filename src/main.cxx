#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <memory>

// for testing
#include <thread>
#include <chrono>
#include <vector>
#include <functional>
#include <sstream>
#include <mutex>

// core
#include "nix/core/db/connection.hxx"
#include "nix/core/logger.hxx"
#include "nix/core/module/api.hxx"
#include "nix/core/module/manager.hxx"
#include "nix/core/object_pool.hxx"
#include "nix/core/program_options.hxx"

// core::net
#include "nix/core/net/transport.hxx"
#include "nix/core/net/transport/options.hxx"

// util
#include "nix/util/fs.hxx"
#include "nix/util/string.hxx"


using nix::core::Logger;
using nix::core::ModuleManager;
using nix::core::ModuleAPI;
using nix::core::ObjectPool;
using nix::core::ProgramOptions;
using nix::core::db::Connection;
using nix::core::net::Transport;
using nix::core::net::transport::Options;

void setup_modules(ModuleManager::Names_t& v,
				   const ProgramOptions& po);


void setup_transport(Options& options,
					 const ProgramOptions& po);


void setup_db_pool(ObjectPool<Connection>& pool,
				   const ProgramOptions& po);


void register_transport_modules(
	std::unique_ptr<Transport>& transport,
	ModuleManager::Names_t& modules);


void test_logger_test(int i, Logger& logger, ObjectPool<Connection>& pool)
{
	std::shared_ptr<Connection> ptr;
	if(pool.acquire(ptr)) {
		std::stringstream s;
		s << "THREAD " << i;
	
		std::this_thread::sleep_for(std::chrono::milliseconds());
		logger.log_info(s.str() + " Finished");
	}
}


int main(int argc, char** argv)
{
	srand(time(NULL));

	std::unique_ptr<Transport> transport;

	try {
		ProgramOptions program_options;
		program_options.parse(argc, argv);
		
		if(program_options.has_help()) {
			program_options.display_help();
			return 0;
		}

		//--------------------------------------------------------------
		Logger logger(program_options);

		// ConnectionPool_t pool_test(
		// 	[]() -> Connection* { 
		// 		return new Connection(1); 
		// 	}
		// );

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

		transport.reset(
			nix::core::net::create_transport(
				Transport::YAMI, transport_options
			)
		);

		register_transport_modules(transport, modules);

		// size_t pool_size = 2;
		// ConnectionPool_t pool(0);
		// for(size_t i = 0 ; i < pool_size; i++) {
		// 	pool.insert(new Connection(i));
		// }

		// std::vector<std::thread> threads;
		// int threads_num = 8;
		// for(int i = 0 ; i < threads_num; i++) {
		// 	threads.push_back(std::thread(logger_test, i, std::ref(logger), std::ref(pool)));
		// }
		// for(int i = 0 ; i < threads_num; i++) {
		// 	threads[i].join();
		// }

		// DBPool db_pool(program_options, logger);
		// PluginStore plugin_store(program_options, logger, db_pool);
		
	}
	catch(std::exception& e) {
		std::cerr << "std::exception (main()): " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	transport->start();

	return EXIT_SUCCESS;
}


void setup_modules(ModuleManager::Names_t& v,
				   const nix::core::ProgramOptions& po)
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
}

void setup_transport(Options& options,
					 const ProgramOptions& po)
{
	using std::string;
	using namespace nix;

	options.port = po.get<int>("TRANSPORT.port");
	options.threads = po.get<int>("TRANSPORT.threads");
	options.listen_address = po.get<string>("TRANSPORT.address_family");

	options.tcp_nonblocking = po.get<bool>("TRANSPORT-YAMI.tcp_nonblocking");
	options.tcp_listen_backlog = po.get<bool>("TRANSPORT-YAMI.tcp_listen_backlog");
	options.dispatcher_threads = po.get<bool>("TRANSPORT-YAMI.dispatcher_threads");

	string address_family(po.get<string>("TRANSPORT.address_family"));
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
	else if(address_family.compare("INPROC") == 0) {
		options.address_family = Options::INPROC;
	}
}

void setup_db_pool(ObjectPool<Connection>& pool,
				   const ProgramOptions& po)
{
}

void register_transport_modules(
	std::unique_ptr<Transport>& transport,
	ModuleManager::Names_t& modules)
{
	if(transport) {
		ModuleManager::Names_t::const_iterator it = modules.begin();
		for( ; it != modules.end(); it++) {
			//transport->register_module(it->get());
		}
	}
}
