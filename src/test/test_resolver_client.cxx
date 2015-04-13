#include "tools/util.hxx"
#include "tools/server.hxx"

#include <vector>
#include <unistd.h>

int main()
{
	using namespace test;
	Configuration config;
	UnitTest unit(config);

	unit.test_case(
		"Basic", 
		[](TestCase& test)
		{
			test::Server server;
			//test.assert_true(server.start());
			server.start();
			sleep(3);

			server.stop();
		}
	);


	return unit.run();
}
