#ifndef NIX_API_MESSAGE_INCOMING_HXX
#define NIX_API_MESSAGE_INCOMING_HXX

#include "nix/api/message.hxx"


namespace nix {
namespace api {


class IncomingMessage : public Message<mongo::BSONObj, mongo::BSONElement>
{
public:
	IncomingMessage() = default;
	virtual ~IncomingMessage() = default;
	virtual bool parse(const std::string& input, bool validate_msg = false);
protected:
	virtual std::string _to_string();
};


} // api
} // nix

#endif
