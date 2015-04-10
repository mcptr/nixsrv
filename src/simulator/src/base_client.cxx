#include "base_client.hxx"


namespace simulator {


BaseClient::BaseClient(const std::string& address,
					   const std::string& module)
	: client_id_(++client_counter_),
	  server_address_(address),
	  module_(module)
{
}

void BaseClient::call(const std::string& route,
					  nix::Message& msg,
					  int timeout)
{
	//LOG(DEBUG) << "call: " << module_ << "::" << route;
	msg.set("@api_key", api_key_);
	yami::parameters params;
	params.set_string("message", msg.to_string());
	
	std::unique_ptr<yami::outgoing_message> om;
	try {
		om = std::move(
			agent_.send(server_address_, module_, route, params));
	}
	catch(const std::exception& e) {
		LOG(ERROR) << "call failed: " << e.what();
		return;
	}
	if(timeout < 0) {
		om->wait_for_completion(max_timeout);
	}
	else {
		om->wait_for_completion(
			timeout > max_timeout ? max_timeout : timeout
		);
	}
	
	const yami::message_state state = om->get_state();
	
	if(state == yami::replied) {
		const yami::parameters& reply = om->get_reply();
		nix::Message m(reply.get_string("message"));
		this->on_replied(m);
	}
	else if(state == yami::rejected) {
		this->on_rejected(om->get_exception_msg());
	}
	else {
		LOG(DEBUG) << "STATE " << state;
		this->on_abanoned();
	}
}

void BaseClient::send_one_way(const std::string& route,
							  nix::Message& msg)
{
	//LOG(DEBUG) << "send_one_way: " << module_ << "::" << route;
	msg.set("@api_key", api_key_);
	yami::parameters params;
	params.set_string("message", msg.to_string());
	
	try {
		agent_.send_one_way(server_address_, module_, route, params);
	}
	catch(const std::exception& e) {
		LOG(ERROR) << "send_one_way failed: " << e.what();
		return;
	}
}

void BaseClient::on_replied(const nix::Message& m)
{
	//LOG(INFO) << "REPLY: " << m.to_string();
}

void BaseClient::on_rejected(const std::string& reason)
{
	//LOG(INFO) << "REJECTED: " << reason;
}

void BaseClient::on_abanoned()
{
	//LOG(WARNING) << "ABANDONED";
}


} // simulator

