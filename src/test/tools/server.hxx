#ifndef NIX_TEST_TOOLS_RESOURCES_SERVER_HXX
#define NIX_TEST_TOOLS_RESOURCES_SERVER_HXX

#include <string>
#include <vector>
#include "util.hxx"


namespace test {

const std::string default_address = "tcp://127.0.0.1";

class Server : public TestDaemon
{
public:
	explicit Server(const std::string& address = default_address);
	explicit Server(const std::vector<std::string>& modules,
					const std::string& address = default_address);

	void set_arguments(std::vector<std::string>& args);
	bool is_ready() const;
	pid_t get_pid() const;

	std::string get_address() const;
	inline const std::string& get_nodename() const
	{
		return nodename_;
	}
protected:

	std::string address_;
	std::string nodename_ = "test-server";
	std::vector<std::string> modules_;
	std::string project_root_;
	std::string pidpath_;
	int port_;
};


} // test


#endif
