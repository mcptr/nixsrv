#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

// core
#include "nix/core/db/connection.hxx"
#include "nix/core/logger.hxx"
#include "nix/core/module/api.hxx"
#include "nix/core/module/manager.hxx"
#include "nix/core/object_pool.hxx"
#include "nix/core/program_options.hxx"
// util
#include "nix/util/fs.hxx"
#include "nix/util/string.hxx"

#include <thread>
#include <chrono>
#include <vector>
#include <functional>
#include <sstream>
#include <mutex>



void read_modules_list(const nix::core::ProgramOptions& po,
					   nix::core::ModuleManager::ModuleList_t& v);


class Connection;

std::mutex test_mtx;

class Connection
{
public:
	Connection();
	Connection(int id) : id_(id) { }
	int  get_id() { return id_; }
	void print() { std::cout << "Connection::print() " << id_ <<  std::endl; }
private:
	int id_;
};

typedef ObjectPool<Connection> ConnectionPool_t;

void logger_test(int i, nix::core::Logger& logger, ConnectionPool_t& pool)
{
	std::shared_ptr<Connection> ptr;
	if(pool.acquire(ptr)) {
		std::stringstream s;
		s << "THREAD " << i;
		ptr->print();
	
		std::this_thread::sleep_for(std::chrono::milliseconds());
		logger.log_info(s.str() + " Finished");
	}
}

int main(int argc, char** argv)
{
	using namespace nix::core;
	using namespace nix::util;

	srand(time(NULL));

	try {
		ProgramOptions program_options;
		program_options.parse(argc, argv);
		
		if(program_options.has_help()) {
			program_options.display_help();
			return 0;
		}

		//--------------------------------------------------------------
		Logger logger(program_options);

		ConnectionPool_t pool_test(
			[]() -> Connection* { 
				return new Connection(1); 
			}
		);

		ModuleManager::ModuleList_t modules;
		read_modules_list(program_options, modules);

		ObjectPool<db::Connection> db_pool; // to be fixed
		ModuleAPI mod_api(db_pool);
		ModuleManager module_manager(mod_api, logger);
		module_manager.load(modules);

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
		
		// Implemenation impl = impl_factory::create(
		// 	impl_name, program_options, logger, db_pool, plugin_store
		// );
	}
	catch(std::exception& e) {
		std::cerr << "std::exception (main()): " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	//impl.serve();

	return EXIT_SUCCESS;
}


void read_modules_list(const nix::core::ProgramOptions& po,
					   nix::core::ModuleManager::ModuleList_t& v)
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
			// if(transport_) {
			// 	loader.load(plugin_name, transport_.get());
			// }
			v.push_back(module_name);
		}
	}
}
