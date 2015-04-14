#ifndef NIX_TEST_TOOLS_RESOURCES_SERVER_HXX
#define NIX_TEST_TOOLS_RESOURCES_SERVER_HXX

#include <string>
#include <vector>
#include "util.hxx"


namespace test {


class Server : public TestDaemon
{
public:
	explicit Server(const std::string& address = "tcp://127.0.0.1");
	explicit Server(const std::vector<std::string>& modules,
					const std::string& address = std::string());

	void set_arguments(std::vector<std::string>& args);
	bool is_ready() const;
	pid_t get_pid() const;

protected:

	std::string address_;
	std::vector<std::string> modules_;
	std::string project_root_;
	std::string pidpath_;
	int port_;
};


} // test


#endif
