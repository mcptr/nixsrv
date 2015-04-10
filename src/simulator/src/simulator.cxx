#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <csignal>
#include <functional>

#include <nix/common.hxx>
#include <nix/util/fs.hxx>

#include "resolver_client.hxx"


std::atomic<bool> signaled = { false };

void termination_signal_handler(int /* sig */)
{
	signaled = true;
}

void run_client(std::shared_ptr<simulator::BaseClient> client)
{
	while(!signaled) {
		try {
			client->run();
		}
		catch(const std::exception& e) {
			LOG(DEBUG) << "run failed: " << e.what();
		}
	}
}

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv)
{
	if(argc < 2) {
		std::cout << "Need server address" << std::endl;
		return 1;
	}

	using namespace simulator;

	std::signal(SIGINT, termination_signal_handler);
	std::signal(SIGTERM, termination_signal_handler);

	std::string base_dir = nix::util::fs::resolve_path(
		nix::util::fs::dirname(argv[0]));
	LOG(DEBUG) << base_dir;

	std::string log_config = nix::util::fs::resolve_path(
		base_dir + "/../log.conf");
	el::Configurations conf(log_config);
	conf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
	el::Loggers::setDefaultConfigurations(conf, true);


	std::vector<std::thread> threads;
	for(int i = 0; i < 1; i++) {
		std::shared_ptr<BaseClient>
			resolver_c(new ResolverClient(argv[1]));

		threads.push_back(
			std::move(
				std::thread(&run_client, resolver_c)));
	}

	for(auto& th : threads) {
		th.join();
	}

	return 0;
}
