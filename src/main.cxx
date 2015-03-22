#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

#include "nix/core/program_options.hxx"
#include "nix/core/logger.hxx"
#include "nix/core/object_pool.hxx"

// testing
#include <thread>
#include <chrono>
#include <vector>
#include <functional>
#include <sstream>
#include <mutex>

class Connection;

std::mutex test_mtx;

class Connection
{
public:
	Connection() = delete;
	Connection(int id) : id_(id) { }
	void print() { std::cout << "Connection::print() " << id_ <<  std::endl; }
private:
	int id_;
};

typedef ObjectPool<Connection> ConnectionPool_t;

void logger_test(nix::core::Logger& logger, ConnectionPool_t& pool)
{
	auto sh_ptr = pool.acquire();
	std::stringstream s;
	s << "THREAD " << std::this_thread::get_id();
	if(sh_ptr) {
		sh_ptr->print();
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	logger.log_info(s.str() + " Finished");
	pool.release(sh_ptr);
}

int main(int argc, char** argv)
{
	using namespace nix::core;

	srand(time(NULL));

	try {
		ProgramOptions program_options;
		program_options.parse(argc, argv);
		
		if(program_options.has_help()) {
			program_options.display_help();
			return 0;
		}

		Logger logger(program_options);
		logger.log_info("Some info message logged");

		ConnectionPool_t pool(
			[]() -> Connection* { return new Connection(666); }
		);

		std::vector<std::thread> threads;
		int threads_num = 5;
		for(int i = 0 ; i < threads_num; i++) {
			threads.push_back(std::thread(logger_test, std::ref(logger), std::ref(pool)));
		}
		for(int i = 0 ; i < threads_num; i++) {
			threads[i].join();
		}

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


