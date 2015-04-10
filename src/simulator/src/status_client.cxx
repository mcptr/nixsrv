#include "status_client.hxx"
#include <sstream>
#include <iomanip>
#include <iostream>


namespace simulator {


void StatusClient::run(const std::string& thread_id)
{
	//LOG(INFO) << "Fetching status";
	nix::Message msg;
	msg.set("stats", true);
	msg.set("module_stats", true);
	call("status", msg);
	if(intval_) {
		std::this_thread::sleep_for(std::chrono::seconds(intval_));
	}
}

void StatusClient::on_replied(const nix::Message& msg)
{
	std::stringstream s;
	std::vector<std::string> header;
	header.push_back("requests");
	header.push_back("auth_errors");
	header.push_back("unroutable");
	header.push_back("rejected");

	s << "\n\n" << "|" << std::left << std::setw(12) << "Module";
	for(auto& h : header) {
		s << "|" << std::left << std::setw(12) << h;
	}
	s << std::endl;

	s << std::setfill('-') << std::setw(72) << "" << std::endl;

	std::vector<std::string> modules;
	modules.push_back("Cache");
	modules.push_back("Resolver");
	modules.push_back("JobQueue");
	modules.push_back("Debug");

	s << std::setfill(' ');
	s << "|" << std::left << std::setw(12) << "Total";
	for(auto& h : header) {
		s << "|" << std::right << std::setw(12) << msg.get("stats." + h, 0);
	}
	s << std::endl;
	s << std::setfill('-') << std::setw(72) << "" << std::endl;
	s << std::setfill(' ');

	for(auto& m : modules) {
		s << "|" << std::left << std::setw(12) << m;
		for(auto& h : header) {
			s << "|" << std::right << std::setw(12) << msg.get("stats." + m + "." + h, 0);
		}
		s << std::endl;
		s << std::setfill('-') << std::setw(72) << "" << std::endl;
		s << std::setfill(' ');
	}
	s << std::endl;

	if(verbose_) {
		LOG(INFO) << s.str();
	}
}

} // simulator
