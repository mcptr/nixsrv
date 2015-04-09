#include "base.hxx"
#include <nix/job.hxx>
#include <string>
#include <iostream>

int main()
{
	using nix::Job;
	using std::string;

	using namespace test;
	Configuration config;
	UnitTest unit(config);

	unit.test_case(
		"Basic", 
		[](TestCase& test)
		{
			Job job_1("action", "{}");
			Job job_2("action", "{}");
			test.assert_not_equal(
				job_1.get_id(),
				job_2.get_id(),
				"Job ids are unique"
			);
		}
	);

	return unit.run();
}
