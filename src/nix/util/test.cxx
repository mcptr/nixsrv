#include <thread>
#include <chrono>
#include <cstdlib>

namespace nix {
namespace util {
namespace test {


void random_thread_sleep()
{
	std::this_thread::sleep_for(
		std::chrono::microseconds(rand() % 1000 + 1)
	);
}


} // test
} // util
} // nix
