#ifndef NIX_MESSAGE_INCOMING_HXX
#define NIX_MESSAGE_INCOMING_HXX

#include <yami4-cpp/yami.h>
#include "nix/message.hxx"


namespace nix {


class IncomingMessage : public Message
{
public:
	IncomingMessage() = delete;
	IncomingMessage(yami::incoming_message& msg);
	virtual ~IncomingMessage() = default;
	void reply();
	void reply(Message& msg);
	void reply_with_error(int error_code, const std::string& msg);
	void reject(const std::string& reason = std::string());
	void reject(int error_code, const std::string& reason = std::string());
protected:
	yami::incoming_message msg_;
};


} // nix

#endif
