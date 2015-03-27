#ifndef NIX_MESSAGE_OUTGOING_HXX
#define NIX_MESSAGE_OUTGOING_HXX

#include "nix/core/message.hxx"


namespace nix {
namespace core {

class Object;
class Array;

class OutgoingMessage : public Message<mongo::BSONObjBuilder, mongo::BSONElement>
{
public:
	OutgoingMessage() = delete;
	OutgoingMessage(const std::string& id, int version = 0);
	explicit OutgoingMessage(const std::string& id, Object& data, int version = 0);
	virtual ~OutgoingMessage() = default;

	void set_content(Object& v);
	void set_content(Array& v);
protected:
	virtual void _set_meta();
	virtual std::string _to_string();
};


} // core
} // nix

#endif
