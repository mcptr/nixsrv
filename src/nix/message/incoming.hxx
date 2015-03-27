#ifndef NIX_MESSAGE_INCOMING_HXX
#define NIX_MESSAGE_INCOMING_HXX

#include "nix/message.hxx"


namespace nix {
namespace core {


class IncomingMessage : public Message<mongo::BSONObj, mongo::BSONElement>
{
public:
	IncomingMessage() = default;
	virtual ~IncomingMessage() = default;
	virtual bool parse(const std::string& input, bool validate_msg = false);
protected:
	virtual std::string _to_string();
};


} // core
} // nix

#endif
