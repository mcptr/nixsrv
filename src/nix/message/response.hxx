#ifndef NIX_MESSAGE_RESPONSE_HXX
#define NIX_MESSAGE_RESPONSE_HXX

#include <memory>
#include <string>
#include <yami4-cpp/yami.h>

#include "nix/common.hxx"
#include "nix/message.hxx"


namespace nix {


class Response : public Message
{
public:
	Response() = delete;
	Response(std::unique_ptr<yami::outgoing_message> om);
	virtual ~Response() = default;

	bool is_replied() const;
	bool is_rejected() const;
	bool is_transmitted() const;
	bool is_abandoned() const;

	bool is_status_ok() const;
	bool is_status_fail() const;

	int get_status_code() const;
	std::string get_status_msg() const;

	const Message& data() const;

protected:
	std::unique_ptr<yami::outgoing_message> yami_om_;
	Message msg_;
	std::string exception_msg_;
};


} // nix

#endif
