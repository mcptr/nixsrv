#include "tools/util.hxx"
#include "tools/server.hxx"
#include "tools/constants.hxx"

#include <vector>
#include <unistd.h>
#include <memory>

#include <nix/common.hxx>
#include <nix/core/client.hxx>
#include <nix/message.hxx>


int main(int argc, char** argv)
{
	using namespace test;
	UnitTest unit_test;

	std::unique_ptr<Server> server(new Server());
	std::string server_address = server->get_address();

	unit_test.test_case(
		"Full status",
		[&server_address](TestCase& test)
		{
			test.assert_true(server_address.length(), "got server address");
			nix::core::Client client;
			test.assert_true(client.ping(server_address), "ping");

			nix::Message om;
			om.set_meta("api_key", DEVELOPMENT_KEY);
			om.set("full", true);
			auto response = client.call(server_address, "Status", "status", om, 1000);
			test.assert_true(response->is_replied(), "replied");
			test.assert_true(response->is_status_ok(), "status OK");
			std::string full_status = response->data().to_string();
			//std::cout << full_status << std::endl;
			test.assert_true(full_status.length(), "got some response");
		}
	);


	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run(argc, argv);
}
