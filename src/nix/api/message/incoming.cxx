#include "incoming.hxx"


namespace nix {
namespace api {


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


} // api
} // nix
