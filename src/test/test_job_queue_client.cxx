#include "tools/util.hxx"
#include "tools/server.hxx"
#include "tools/constants.hxx"
#include "tools/init_helpers.hxx"

#include <vector>
#include <unistd.h>
#include <memory>
#include <map>

#include <nix/common.hxx>
#include <nix/core/job_queue_client.hxx>
#include <nix/message.hxx>



int main(int argc, char** argv)
{
	using namespace test;
	UnitTest unit_test;

	std::vector<std::string> modules;
	modules.push_back("job-queue");

	std::unique_ptr<Server> server(new Server(modules));

	const std::string server_address = server->get_address();
	const std::string server_nodename = server->get_nodename();

	//----------------------------------------------------------------
	// test data
	std::string job_module = "test_module";
	std::string job_action = "test_action";
	std::string job_param = "this is a test";
	nix::ClientJob job;
	job.set_module(job_module);
	job.set_action(job_action);
	job.parameters().set("test", job_param);

	std::string job_id;

	nix::Message result;
	result.set("fake_result", 123);

	// end of test data
	//----------------------------------------------------------------

	unit_test.test_case(
		"Ping",
		[&server_address, &server_nodename](TestCase& test)
		{
			auto client = 
				init_service_client<nix::core::JobQueueClient>(
					server_address, server_nodename);

			test.assert_true(client->ping_service(), "service alive");
		}
	);

	unit_test.test_case(
		"Create test queue",
		[&server_address, &server_nodename, &job_module](TestCase& test)
		{
			auto client = 
				init_service_client<nix::core::JobQueueClient>(
					server_address, server_nodename);

			nix::Message create_msg;
			create_msg.set_meta("api_key", DEVELOPMENT_KEY_ADMIN);
			create_msg.set("queue", job_module);
			create_msg.set("create", true);

			auto response = client->call(
				server_address, "JobQueue", "manage", create_msg);
			test.assert_true(response->is_status_ok(), "create success");
		}
	);

	unit_test.test_case(
		"Submit job",
		[&server_address, &server_nodename, &job, &job_id](TestCase& test)
		{
			auto client = 
				init_service_client<nix::core::JobQueueClient>(
					server_address, server_nodename);

			auto response = client->submit(job);
			test.assert_true(response->is_status_ok(), "submit call");
			job_id = response->get_meta("job_id", std::string());
			test.assert_true(job_id.length(), "got job id");
		}
	);

	// unit_test.test_case(
	// 	"Get work",
	// 	[&server_address, &server_nodename, &job,
	// 	 &job_module, &job_action, &job_id](TestCase& test)
	// 	{
	// 		auto client = 
	// 			init_service_client<nix::core::JobQueueClient>(
	// 				server_address, server_nodename);

	// 		auto response = client->get_work(job_module);
	// 		test.assert_true(response->is_status_ok(), "get_work called");
	// 		// we have one job only, so we can test job_id
	// 		std::string received_job_id =
	// 			response->get_meta("job_id", std::string());

	// 		test.assert_equal(
	// 			received_job_id, job_id, "received correct job");

	// 		test.assert_equal(
	// 			response->get_meta("queue_node", std::string()),
	// 			server_nodename,
	// 			"received job points to origin node");

	// 		test.assert_equal(
	// 			response->get("action", std::string()),
	// 			job_action,
	// 			"correct job action");

	// 		test.assert_equal(
	// 			response->get_raw_value("parameters"),
	// 			job.get_raw_value("parameters"),
	// 			"correct job parameters");
	// 	}
	// );

	// unit_test.test_case(
	// 	"Progress set/get",
	// 	[&server_address, &server_nodename, &job, &job_id](TestCase& test)
	// 	{
	// 		auto client = 
	// 			init_service_client<nix::core::JobQueueClient>(
	// 				server_address, server_nodename);

	// 		job->set_progress(0.678);

	// 		bool success = client->set_progress(job);
	// 		test.assert_true(response->is_status_ok(), "submit call");
	// 		job_id = response->get_meta("job_id", std::string());
	// 		test.assert_true(job_id.length(), "got job id");
	// 	}
	// );

	// unit_test.test_case(
	// 	"Set result",
	// 	[&server_address, &server_nodename, &job_id, &result](TestCase& test)
	// 	{
	// 		auto client = 
	// 			init_service_client<nix::core::JobQueueClient>(
	// 				server_address, server_nodename);

	// 		bool success = client->set_result(job->);
	// 		test.assert_true(response->is_status_ok(), "submit call");
	// 		job_id = response->get_meta("job_id", std::string());
	// 		test.assert_true(job_id.length(), "got job id");
	// 	}
	// );


	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run(argc, argv);
}
