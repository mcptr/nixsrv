#ifndef NIX_API_RESPONSE_HXX
#define NIX_API_RESPONSE_HXX

#include <string>
#include <memory>
#include "nix/api/message.hxx"
#include "nix/api/message/outgoing.hxx"


namespace nix {
namespace api {


class Response
{
public:
	Response(int request_id);
	virtual ~Response();
	OutgoingMessage& from_schema(const std::string& id, int version = 0);

	OutgoingMessage& create(
		const std::string& id,
		int version = 0
	);

	OutgoingMessage& create(
		Object& msg,
		const std::string& id,
		int version = 0
	);

	OutgoingMessage& create(
		Array& msg,
		const std::string& id,
		int version = 0
	);

	std::string to_string();

	void set_content(OutgoingMessage* msg);
	void set_content(Object& obj);
	void set_content(Array& arr);
	void set_content(Object& obj, const std::string& id, int version = 0);
	void set_content(Array& arr, const std::string& id, int version = 0);

protected:
	std::unique_ptr<OutgoingMessage> message_;
	int request_id_;
};


} // api
} // nix

#endif
