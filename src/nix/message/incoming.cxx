#include <exception>
#include "nix/common.hxx"

#include "incoming.hxx"


namespace nix {


IncomingMessage::IncomingMessage(yami::incoming_message& im)
	: Message(),
	  msg_(im)
{
}

void IncomingMessage::reply()
{
	try {
		clear();
		set_status(nix::ok);
		reply(*this);
	}
	catch(yami::yami_runtime_error& e) {
		LOG(DEBUG) << "Exception: " << e.what();
	}
}

void IncomingMessage::reply(Message& msg, nix::StatusCode_t status)
{
	msg.set_status_code(status);
	yami::parameters params;
	params.set_string("message", msg.to_string());

	try {
		msg_.reply(params);
	}
	catch(yami::yami_runtime_error& e) {
		LOG(DEBUG) << "Exception: " << e.what();
	}
}

void IncomingMessage::reply(const Job& job, nix::StatusCode_t status)
{
	Message msg;
	msg.parse(job.to_string());
	this->reply(msg, status);
}

void IncomingMessage::reply_with_error(nix::StatusCode_t error_code,
									   const std::string& msg)
{
	clear();
	set_status(error_code, msg);
	reply(*this);
}

void IncomingMessage::fail(const std::string& reason)
{
	clear();
	fail(nix::fail, reason);
}

void IncomingMessage::fail(nix::StatusCode_t error_code,
						   const std::string& reason)
{
	LOG(DEBUG) << "Fail: " << reason;
	clear();
	set_status(error_code, reason);

	yami::parameters params;
	params.set_string("message", to_string());

	try {
		msg_.reply(params);
	}
	catch(yami::yami_runtime_error& e) {
		LOG(DEBUG) << "Exception: " << e.what();
	}
}

void IncomingMessage::reject(const std::string& reason)
{
	try {
		msg_.reject(reason);
	}
	catch(yami::yami_runtime_error& e) {
		LOG(DEBUG) << "Exception: " << e.what();
	}
}

void IncomingMessage::reject(nix::StatusCode_t status, const std::string& reason)
{
	std::string full_reason = std::to_string(status) + ": " + reason;
	msg_.reject(full_reason);
}


} // nix
