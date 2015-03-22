#include <unistd.h>
#include <cstdlib>
#include <ctime>

#include "nix/core/program_options.hxx"
#include "nix/core/logger.hxx"

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

		Logger logger;
		//logger.log(Logger::Level::LOG_LVL_DEBUG, "test", false) << "dupa" << "kupa";
		logger.log_info("Some info message logged");
		// Logger logger(program_options);
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


