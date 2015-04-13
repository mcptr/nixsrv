#include "response.hxx"
#include "nix/status_code.hxx"


namespace nix {


Response::Response(std::unique_ptr<yami::outgoing_message> om)
	: yami_om_(std::move(om))
{
	const yami::message_state state = yami_om_->get_state();
	if(state == yami::replied) {
		const yami::parameters& reply = yami_om_->get_reply();
		msg_.parse(reply.get_string("message"));
	}
	else if(state == yami::rejected) {
		exception_msg_ = yami_om_->get_exception_msg();
	}
}

bool Response::is_replied() const
{
	return (yami_om_->get_state() == yami::replied);
}

bool Response::is_rejected() const
{
	return (yami_om_->get_state() == yami::rejected);
}

bool Response::is_transmitted() const
{
	return (yami_om_->get_state() == yami::transmitted);
}

bool Response::is_abandoned() const
{
	return (yami_om_->get_state() == yami::abandoned);
}

bool Response::is_status_ok() const
{
	return (msg_.get("@status_code", nix::null_value) == nix::ok);
}

bool Response::is_status_fail() const
{
	return (msg_.get("@status_code", nix::ok) != nix::ok);
}

int Response::get_status_code() const
{
	return msg_.get("@status_code", nix::null_value);
}

std::string Response::get_status_msg() const
{
	return msg_.get("@status_msg", "");
}

const Message& Response::data() const
{
	return msg_;
}


} // nix
