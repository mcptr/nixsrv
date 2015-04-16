#include "job.hxx"


namespace nix {


Job::Job(const nix::Message& msg)
{
	deserialize(msg);
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

void Job::deserialize(const Message& msg)
{
	std::string empty_str;
	id_ = msg.get_meta("job_id", empty_str);
	module_ = msg.get("module", empty_str);
	action_ = msg.get("action", empty_str);
	origin_node_ = msg.get("origin_node", empty_str);
	progress_ = msg.get("progress", 0.0);
	ctime_ = msg.get("ctime", 0);
	completed_ = msg.get("completed", false);
	parameters_ = msg.get_raw_value("parameters");
}

void Job::set_origin_node(const std::string& nodename)
{
	origin_node_ = nodename;
}


} // nix
