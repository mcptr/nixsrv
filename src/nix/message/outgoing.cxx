#include "outgoing.hxx"
#include "object.hxx"
#include "array.hxx"


namespace nix {
namespace core {


OutgoingMessage::OutgoingMessage(const std::string& id, int version)
	: Message<mongo::BSONObjBuilder, mongo::BSONElement>(id, version)
{
	this->_set_meta();
}

OutgoingMessage::OutgoingMessage(const std::string& id, Object& data, int version)
	: Message<mongo::BSONObjBuilder, mongo::BSONElement>(id, version)
{
	this->_set_meta();
	this->set_content(data);
}

void OutgoingMessage::set_content(Object& v)
{
	this->content_.append("message", v.get_obj());
}
	
void OutgoingMessage::set_content(Array& v)
{
	this->content_.appendArray("message", v.get_obj());
}

void OutgoingMessage::_set_meta()
{
	this->content_.append("@id", this->get_id());
	this->content_.append("@version", this->get_version());
}

std::string OutgoingMessage::_to_string()
{
	return this->content_.obj().jsonString();
}


} // core
} // nix

