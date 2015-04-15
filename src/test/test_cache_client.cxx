#include "tools/util.hxx"
#include "tools/server.hxx"
#include "tools/constants.hxx"

#include <vector>
#include <unistd.h>
#include <memory>
#include <map>

#include <nix/common.hxx>
#include <nix/core/cache_client.hxx>
#include <nix/message.hxx>


struct Stats
{
	int writes = 0;
	int writes_failed = 0;
	int removals = 0;
	int hits = 0;
	int hits_expired = 0;
	int misses = 0;
	int total_calls = 0;
	int cache_size = 0;
};

int main(int argc, char** argv)
{
	using namespace test;
	UnitTest unit_test;

	std::vector<std::string> modules;
	modules.push_back("cache");

	std::unique_ptr<Server> server(new Server(modules));

	std::string server_address = server->get_address();

	//----------------------------------------------------------------
	// test data
	std::string test_key = "test";

	nix::Message test_value;
	test_value.set("a_string", "some string");
	test_value.set("an_int", 1234);
	test_value.set_null("nested.null");
	test_value.set("nested.string", "hello");

	Stats stats;
	// end of test data
	//----------------------------------------------------------------

	unit_test.test_case(
		"Ping",
		[&server_address](TestCase& test)
		{
			nix::core::CacheClient client(
				server_address, DEVELOPMENT_KEY, 1000);

			test.assert_true(client.ping_service(), "service alive");
		}
	);

	unit_test.test_case(
		"Store",
		[&server_address, &test_key, &test_value, &stats](TestCase& test)
		{
			nix::core::CacheClient client(
				server_address, DEVELOPMENT_KEY, 1000);

			bool success = client.store(test_key, test_value);
			test.assert_true(success, "store succeeded");
			stats.writes++;
			stats.cache_size++;
		}
	);

	unit_test.test_case(
		"Retrieve",
		[&server_address, &test_key, &test_value, &stats](TestCase& test)
		{
			nix::core::CacheClient client(
				server_address, DEVELOPMENT_KEY, 1000);

			nix::Message response_msg;
			bool success = client.retrieve(test_key, response_msg);
			test.assert_true(success, "retrieve succeeded");
			test.assert_equal(
				response_msg.to_string(),
				test_value.to_string(),
				"retrieved value"
			);
			stats.hits++;
		}
	);

	unit_test.test_case(
		"Remove",
		[&server_address, &test_key, &stats](TestCase& test)
		{
			nix::core::CacheClient client(
				server_address, DEVELOPMENT_KEY, 1000);

			bool success = client.remove(test_key);
			test.assert_true(success, "remove succeeded");
			stats.removals++;
			stats.cache_size--;

			nix::Message response_msg;
			success = client.retrieve(test_key, response_msg);
			test.assert_false(success, "retrieve fails after removal");
			stats.misses++;
		}
	);

	unit_test.test_case(
		"Status",
		[&server_address, &stats](TestCase& test)
		{
			nix::core::CacheClient client(
				server_address, DEVELOPMENT_KEY, 1000);

			nix::Message result;
			test.assert_true(client.status(result), "status call");

			std::map<std::string, int> stats_map;
			stats_map.emplace("writes", stats.writes);
			stats_map.emplace("writes_failed", stats.writes_failed);
			stats_map.emplace("removals", stats.removals);
			stats_map.emplace("hits", stats.hits);
			stats_map.emplace("hits_expired", stats.hits_expired);
			stats_map.emplace("misses", stats.misses);

			int total = 0;
			for(auto& st : stats_map) {
				total += st.second;
			}			

			stats_map.emplace("total_calls", total);
			stats_map.emplace("cache_size", stats.cache_size);

			for(auto& st : stats_map) {
				test.assert_equal(
					result.get(st.first, -1),
					st.second,
					"status: " + st.first
				);
			}
		}
	);

	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run(argc, argv);
}
