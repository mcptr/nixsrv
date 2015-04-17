#include "job.hxx"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>
#include "nix/common.hxx"

namespace nix {


Job::Job()
{
	init();
}

Job::Job(const nix::Message& msg)
	: Job()
{
	from_message(msg);
	init();
}

void Job::init()
{
	if(id_.empty()) {
		set_id();
	}

	if(!ctime_) {
		ctime_ = std::time(nullptr);
	}
}

std::string Job::to_string() const
{
	Message out;
	out.set_meta("job_id", id_);
	out.set_meta("origin_node", origin_node_);
	out.set_meta("api_key", api_key_);
	out.set("module", module_);
	out.set("action", action_);
	out.set("ctime", ctime_);
	out.set("progress", progress_);
	out.set("completed", completed_);
	out.set("parameters", parameters_);
	return out.to_string();
}

void Job::from_message(const Message& msg)
{
	std::string empty_str;
	id_ = msg.get_meta("job_id", empty_str);
	api_key_ = msg.get_meta("api_key", empty_str);
	origin_node_ = msg.get_meta("origin_node", empty_str);
	module_ = msg.get("module", empty_str);
	action_ = msg.get("action", empty_str);
	ctime_ = msg.get("ctime", 0);
	progress_ = msg.get("progress", 0.0);
	completed_ = msg.get("completed", false);
	parameters_ = msg.get_raw_value("parameters");
}

void Job::deserialize(const std::string& serialized)
{
	Message msg;
	msg.parse(serialized);
	from_message(msg);
}

void Job::set_origin_node(const std::string& nodename)
{
	origin_node_ = nodename;
}

void Job::set_api_key(const std::string& api_key)
{
	api_key_ = api_key;
}

void Job::set_id(const std::string& id)
{
	if(id.length()) {
		id_ = id;
	}
	else {
		static boost::uuids::random_generator rg;
		std::stringstream s;
		s << rg();
		id_ = s.str();
	}
}


void Job::set_progress(double progress)
{
	if(progress >= 100.0) {
		completed_ = true;
		progress_ = 100.0;
	}
	else {
		progress_ = progress;
	}
}

void Job::set_module(const std::string& module)
{
	module_ = module;
}

void Job::set_action(const std::string& action)
{
	action_ = action;
}

bool Job::is_completed() const
{
	return completed_;
}


} // nix
