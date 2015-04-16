#include "client_job.hxx"
#include <ctime>


namespace nix {


ClientJob::ClientJob() : Job()
{
	ctime_ = std::time(nullptr);
}

ClientJob::ClientJob(const std::string& module,
					 const std::string& action)
	: ClientJob()
{
	module_ = module;
	action_ = action;
}

ClientJob::ClientJob(const nix::Message& msg)
	: ClientJob()
{
	deserialize(msg);
}

void ClientJob::set_module(const std::string& module)
{
	module_ = module;
}

void ClientJob::set_action(const std::string& action)
{
	action_ = action;
}
	
void ClientJob::set_api_key(const std::string& api_key)
{
	api_key_ = api_key;
}


} // nix
