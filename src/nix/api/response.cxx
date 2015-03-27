#include "response.hxx"

namespace nix {
namespace api {

Response::Response(int request_id)
	: request_id_(request_id)
{
	message_.reset();
}

Response::~Response()
{
	message_.reset();
}

message::OutgoingMessage& Response::from_schema(
	const std::string& id, int version)
{
	// FIXME: create factory
	message_.reset(new message::OutgoingMessage(id, version));
	return *message_;
}

message::OutgoingMessage& Response::create(const std::string& id,
										   int version)
{
	message_.reset(new message::OutgoingMessage(id, version));
	return *message_;
}

message::OutgoingMessage& Response::create(
	message::Object& msg,
	const std::string& id,
	int version)
{
	message_.reset(new message::OutgoingMessage(id, version));
	message_->set_content(msg);
	return *message_;
}

message::OutgoingMessage& Response::create(
	message::Array& msg,
	const std::string& id,
	int version)
{
	message_.reset(new message::OutgoingMessage(id, version));
	message_->set_content(msg);
	return *message_;
}

std::string Response::to_string()
{
	return (message_ ? message_->to_string() : "");
}

void Response::set_content(message::OutgoingMessage* msg)
{
	message_.reset(msg);
}

void Response::set_content(message::Object& obj)
{
	message_.reset(new message::OutgoingMessage("Generic", 0));
	message_->set_content(obj);
}

void Response::set_content(message::Array& arr)
{
	message_.reset(new message::OutgoingMessage("Generic", 0));
	message_->set_content(arr);
}

void Response::set_content(message::Object& obj, const std::string& id, int version)
{
	this->create(obj, id, version);
}

void Response::set_content(message::Array& arr, const std::string& id, int version)
{
	this->create(arr, id, version);
}


} // api
} // nix
