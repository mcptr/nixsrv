#include "tools/util.hxx"
#include "tools/server.hxx"
#include "tools/constants.hxx"

#include <vector>
#include <unistd.h>
#include <memory>
#include <map>

#include <nix/common.hxx>
#include <nix/core/resolver_client.hxx>
#include <nix/message.hxx>


int main(int argc, char** argv)
{
	using namespace test;
	UnitTest unit_test;

	std::vector<std::string> modules;
	modules.push_back("resolver");

	std::unique_ptr<Server> server(new Server(modules));

	std::string server_address = server->get_address();

	//----------------------------------------------------------------
	// test data

	std::string self_nodename = "test-node-5";
	std::string self_service = "test-service";
	std::string self_address = "tcp://test-host:23456";

	// end of test data
	//----------------------------------------------------------------


	unit_test.test_case(
		"Ping",
		[&server_address, &self_nodename](TestCase& test)
		{
			test.assert_true(server_address.length(), "got server address");
			nix::core::ResolverClient client(
				server_address, self_nodename, DEVELOPMENT_KEY);
			test.assert_true(client.ping_service(), "service alive");
		}
	);

	unit_test.test_case(
		"Bind nodes",
		[&server_address, &self_nodename, &self_address](TestCase& test)
		{
			nix::core::ResolverClient client(
				server_address, self_nodename, DEVELOPMENT_KEY);

			bool success = client.bind_node(self_address);
			test.assert_true(success, "bind succeeded");
		}
	);

	unit_test.test_case(
		"Resolve nodes",
		[&server_address, &self_nodename, &self_address](TestCase& test)
		{
			nix::core::ResolverClient client(
				server_address, self_nodename, DEVELOPMENT_KEY);

			std::string resolved = client.resolve_node(self_nodename);
			test.assert_equal(resolved, self_address, "resolved correct node");
		}
	);

	unit_test.test_case(
		"Unbind nodes",
		[&server_address, &self_nodename](TestCase& test)
		{
			std::string empty_str;
			nix::core::ResolverClient client(
				server_address, self_nodename, DEVELOPMENT_KEY);

			bool success = client.unbind_node();
			test.assert_true(success, "unbind call successful");
			std::string resolved = client.resolve_node(self_nodename);
			test.assert_equal(resolved, empty_str, "node unbound");
		}
	);

	// ---------------------------------------------------------------
	// Services tests.
	// ----------------------------------------------------------------

	unit_test.test_case(
		"Bind nodes",
		[&server_address, &self_nodename, &self_address](TestCase& test)
		{
			nix::core::ResolverClient client(
				server_address, self_nodename, DEVELOPMENT_KEY);

			bool success = client.bind_node(self_address);
			test.assert_true(success, "bind succeeded");
		}
	);

	unit_test.test_case(
		"Bind services",
		[&server_address, &self_nodename, &self_service](TestCase& test)
		{
			nix::core::ResolverClient client(
				server_address, self_nodename, DEVELOPMENT_KEY);

			bool success = client.bind_service(self_service);
			test.assert_true(success, "bind service succeeded");
		}
	);

	unit_test.test_case(
		"Resolve service",
		[&server_address, &self_nodename, &self_address, &self_service](TestCase& test)
		{
			nix::core::ResolverClient client(
				server_address, self_nodename, DEVELOPMENT_KEY);

			nix::Message::Array_t resolved = client.resolve_service(self_service);
			std::string address = resolved.get_value()[0].asString();
			test.assert_equal(
				address, self_address, "resolved correct service");
		}
	);

	unit_test.test_case(
		"Unbind service (self nodename)",
		[&server_address, &self_nodename, &self_service](TestCase& test)
		{
			nix::core::ResolverClient client(
				server_address, self_nodename, DEVELOPMENT_KEY);

			bool success = client.unbind_service(self_service);
			test.assert_true(success, "unbind service successful");
			nix::Message::Array_t resolved = client.resolve_service(self_service);
			test.assert_true(resolved.get_value().empty(), "service unbound");
		}
	);

	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run(argc, argv);
}
