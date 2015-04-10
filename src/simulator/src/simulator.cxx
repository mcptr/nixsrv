#include <memory>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <csignal>
#include <functional>
#include <string>
#include <sstream>
#include <cstdlib>

#include <nix/common.hxx>
#include <nix/util/fs.hxx>

#include "resolver_client.hxx"
#include "cache_client.hxx"
#include "status_client.hxx"


std::atomic<bool> signaled = { false };

void termination_signal_handler(int /* sig */)
{
	signaled = true;
}

void run_client(std::shared_ptr<simulator::BaseClient> client)
{

	std::stringstream thread_id;
	thread_id << std::this_thread::get_id();
	while(!signaled) {
		// std::this_thread::sleep_for(
		// 	std::chrono::microseconds(rand() % 5000 + 10)
		// );
		client->run(thread_id.str());
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

	std::string log_config = nix::util::fs::resolve_path(
		base_dir + "/../log.conf");
	el::Configurations conf(log_config);
	conf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
	el::Loggers::setDefaultConfigurations(conf, true);


	std::vector<std::thread> threads;
	for(int i = 0; i < 16; i++) {
		std::vector<std::shared_ptr<BaseClient>> clients;
		clients.push_back(
			std::shared_ptr<BaseClient>(new ResolverClient(argv[1])));
		clients.push_back(
			std::shared_ptr<BaseClient>(new CacheClient(argv[1])));
		clients.push_back(
			std::shared_ptr<BaseClient>(new StatusClient(argv[1], false)));

		for(auto& client : clients) {
			threads.push_back(
				std::move(
					std::thread(&run_client, client)));
		}
	}

	std::shared_ptr<BaseClient>
		monitor(new StatusClient(argv[1], true, 1));
	threads.push_back(std::move(std::thread(&run_client, monitor)));

	for(auto& th : threads) {
		th.join();
	}

	return 0;
}
