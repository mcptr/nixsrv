#ifndef NIX_TEST_TOOLS_RESOURCES_SERVER_HXX
#define NIX_TEST_TOOLS_RESOURCES_SERVER_HXX

#include <string>
#include <vector>


namespace test {


class Server
{
public:
	explicit Server(const std::string& address = "tcp://127.0.0.1");
	explicit Server(const std::vector<std::string>& modules,
					const std::string& address = std::string());

	bool start();
	void stop();

protected:
	const std::string address_;
	std::vector<std::string> modules_;
	std::string project_root_;
	std::string pidpath_;
	int port_;

	bool is_running_ = false;
};


} // test


#endif
