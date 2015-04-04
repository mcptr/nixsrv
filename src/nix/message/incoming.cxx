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
		msg_.reply();
	}
	catch(yami::yami_runtime_error& e) {
		LOG(DEBUG) << "Exception: " << e.what();
	}
}

void IncomingMessage::reply(Message& msg)
{
	yami::parameters params;
	params.set_string("message", msg.to_string());
	try {
		msg_.reply(params);
	}
	catch(yami::yami_runtime_error& e) {
		LOG(DEBUG) << "Exception: " << e.what();
	}
}

void IncomingMessage::reply_with_error(nix::StatusCode_t error_code,
									   const std::string& msg)
{
	set_status(error_code, msg);
	reply(*this);
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

void IncomingMessage::reject(nix::StatusCode_t error_code,
							 const std::string& reason)
{
	reject(std::to_string(error_code) + ": " + reason);
}

} // nix
