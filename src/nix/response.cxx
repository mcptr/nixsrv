#include "response.hxx"


namespace nix {


Response::Response(int request_id)
	: request_id_(request_id)
{
	message_.reset();
}

Response::~Response()
{
	message_.reset();
}

OutgoingMessage& Response::from_schema(
	const std::string& id, int version)
{
	// FIXME: create factory
	message_.reset(new OutgoingMessage(id, version));
	return *message_;
}

OutgoingMessage& Response::create(const std::string& id,
										   int version)
{
	message_.reset(new OutgoingMessage(id, version));
	return *message_;
}

OutgoingMessage& Response::create(
	Object& msg,
	const std::string& id,
	int version)
{
	message_.reset(new OutgoingMessage(id, version));
	message_->set_content(msg);
	return *message_;
}

OutgoingMessage& Response::create(
	Array& msg,
	const std::string& id,
	int version)
{
	message_.reset(new OutgoingMessage(id, version));
	message_->set_content(msg);
	return *message_;
}

std::string Response::to_string()
{
	return (message_ ? message_->to_string() : "");
}

void Response::set_content(OutgoingMessage* msg)
{
	message_.reset(msg);
}

void Response::set_content(Object& obj)
{
	message_.reset(new OutgoingMessage("Generic", 0));
	message_->set_content(obj);
}

void Response::set_content(Array& arr)
{
	message_.reset(new OutgoingMessage("Generic", 0));
	message_->set_content(arr);
}

void Response::set_content(Object& obj, const std::string& id, int version)
{
	this->create(obj, id, version);
}

void Response::set_content(Array& arr, const std::string& id, int version)
{
	this->create(arr, id, version);
}


} // nix
