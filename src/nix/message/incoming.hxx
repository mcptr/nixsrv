#ifndef NIX_MESSAGE_INCOMING_HXX
#define NIX_MESSAGE_INCOMING_HXX

#include <yami4-cpp/yami.h>
#include "nix/message.hxx"
#include "nix/message/outgoing.hxx"


namespace nix {


class IncomingMessage : public Message<mongo::BSONObj, mongo::BSONElement>
{
public:
	IncomingMessage() = delete;
	IncomingMessage(yami::incoming_message& msg);
	virtual ~IncomingMessage() = default;
	virtual bool parse(const std::string& input, bool validate_msg = false);
	void reply(OutgoingMessage& msg);
	void reply();
	void reject();
protected:
	yami::incoming_message msg_;
	virtual std::string _to_string();
};


} // nix

#endif
