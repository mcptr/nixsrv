#include "tools/util.hxx"
#include "tools/server.hxx"
#include "tools/constants.hxx"
#include "tools/init_helpers.hxx"

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
	std::string server_nodename = server->get_nodename();

	//----------------------------------------------------------------
	// test data

	std::string test_service = "test-service";

	// end of test data
	//----------------------------------------------------------------


	unit_test.test_case(
		"Ping",
		[&server_address, &server_nodename](TestCase& test)
		{
			auto client = 
				init_service_client<nix::core::ResolverClient>(
					server_address, server_nodename);

			test.assert_true(server_address.length(), "got server address");
			test.assert_true(client->ping_service(), "service alive");
		}
	);

	unit_test.test_case(
		"Bind nodes",
		[&server_address, &server_nodename](TestCase& test)
		{
			auto client = 
				init_service_client<nix::core::ResolverClient>(
					server_address, server_nodename);

			bool success = client->bind_node();
			test.assert_true(success, "bind succeeded");
		}
	);

	unit_test.test_case(
		"Resolve node",
		[&server_address, &server_nodename](TestCase& test)
		{
			auto client = 
				init_service_client<nix::core::ResolverClient>(
					server_address, server_nodename);

			std::string resolved = client->resolve_node(server_nodename);
			test.assert_equal(resolved, server_address, "resolved correct node");
		}
	);

	unit_test.test_case(
		"Unbind nodes",
		[&server_address, &server_nodename](TestCase& test)
		{
			auto client = 
				init_service_client<nix::core::ResolverClient>(
					server_address, server_nodename);

			std::string empty_str;

			bool success = client->unbind_node();
			test.assert_true(success, "unbind call successful");
			std::string resolved = client->resolve_node(server_nodename);
			test.assert_equal(resolved, empty_str, "node unbound");
		}
	);

	// ---------------------------------------------------------------
	// Services tests.
	// ----------------------------------------------------------------

	unit_test.test_case(
		"Bind nodes",
		[&server_address, &server_nodename](TestCase& test)
		{
			auto client = 
				init_service_client<nix::core::ResolverClient>(
					server_address, server_nodename);

			bool success = client->bind_node();
			test.assert_true(success, "bind succeeded");
		}
	);

	unit_test.test_case(
		"Bind services",
		[&server_address, &server_nodename, &test_service](TestCase& test)
		{
			auto client = 
				init_service_client<nix::core::ResolverClient>(
					server_address, server_nodename);

			bool success = client->bind_service(test_service);
			test.assert_true(success, "bind service succeeded");
		}
	);

	unit_test.test_case(
		"Resolve service",
		[&server_address, &server_nodename, &test_service](TestCase& test)
		{
			auto client = 
				init_service_client<nix::core::ResolverClient>(
					server_address, server_nodename);

			nix::Message::Array_t resolved = client->resolve_service(test_service);
			std::string address = resolved.get_value()[0].asString();
			test.assert_equal(
				address, server_address, "resolved correct service");
		}
	);

	unit_test.test_case(
		"Unbind service (self nodename)",
		[&server_address, &server_nodename, &test_service](TestCase& test)
		{
			auto client = 
				init_service_client<nix::core::ResolverClient>(
					server_address, server_nodename);

			bool success = client->unbind_service(test_service);
			test.assert_true(success, "unbind service successful");
			nix::Message::Array_t resolved = client->resolve_service(test_service);
			test.assert_true(resolved.get_value().empty(), "service unbound");
		}
	);

	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run(argc, argv);
}
