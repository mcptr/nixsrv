#include "incoming.hxx"


namespace nix {


IncomingMessage::IncomingMessage(yami::incoming_message& im)
	: msg_(im)
{
}

bool IncomingMessage::parse(const std::string& input, bool validate_msg)
{
	bool valid = false;
	this->content_ = mongo::fromjson(input);
	valid = this->content_.isValid();
	if(valid && validate_msg) {
		valid = this->validate();
	}

	if(valid) {
		this->id_ = this->get_string("@id");
		this->version_ = this->get_integer("@version");
	}

	return valid;
}

std::string IncomingMessage::_to_string()
{
	return this->content_.jsonString();
}

void IncomingMessage::reply(OutgoingMessage& msg)
{
	yami::parameters params;
	params.set_string("message", msg.to_string());
	msg_.reply(params);
}

void IncomingMessage::reply()
{
	msg_.reply();
}

void IncomingMessage::reject()
{
	msg_.reject();
}


} // nix
