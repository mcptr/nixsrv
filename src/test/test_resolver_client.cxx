#include "tools/util.hxx"
#include "tools/server.hxx"

#include <nix/common.hxx>
#include <vector>
#include <unistd.h>
#include <memory>

int main()
{
	using namespace test;
	UnitTest unit_test;
	std::unique_ptr<Server> server(new Server());

	unit_test.test_case(
		"Basic", 
		[](TestCase& test)
		{
		}
	);


	ProcessTest proc_test(std::move(server), unit_test);
	return proc_test.run();
}
