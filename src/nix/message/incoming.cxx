#include "incoming.hxx"
#include "nix/common.hxx"
#include <exception>

namespace nix {


IncomingMessage::IncomingMessage(yami::incoming_message& im)
	: Message(),
	  msg_(im)
{
}

void IncomingMessage::reply()
{
	try {
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

void IncomingMessage::reply_with_error(int error_code,
									   const std::string& msg)
{
	set_error(error_code, msg);
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

void IncomingMessage::reject(int error_code, const std::string& reason)
{
	reject(std::to_string(error_code) + ": " + reason);
}

} // nix
