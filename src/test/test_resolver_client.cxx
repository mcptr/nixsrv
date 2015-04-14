#include "tools/util.hxx"
#include "tools/server.hxx"
#include "tools/constants.hxx"

#include <vector>
#include <unistd.h>
#include <memory>

#include <nix/common.hxx>
#include <nix/core/resolver_client.hxx>


int main()
{
	using namespace test;
	UnitTest unit_test;

	std::vector<std::string> modules;
	modules.push_back("resolver");

	std::unique_ptr<Server> server(new Server(modules));

	std::string server_address = server->get_address();

	unit_test.test_case(
		"Ping",
		[&server_address](TestCase& test)
		{
			test.assert_true(server_address.length(), "got server address");
			nix::core::ResolverClient client(server_address, DEVELOPMENT_KEY);
			test.assert_true(client.ping_service(), "service alive");

			nix::Message om;
			om.set_meta("api_key", DEVELOPMENT_KEY);
		}
	);


	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run();
}
