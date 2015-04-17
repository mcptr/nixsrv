#ifndef NIX_MESSAGE_INCOMING_HXX
#define NIX_MESSAGE_INCOMING_HXX

#include <yami4-cpp/yami.h>

#include "nix/common.hxx"
#include "nix/message.hxx"
#include "nix/job.hxx"


namespace nix {


class IncomingMessage : public Message
{
public:
	IncomingMessage() = delete;
	IncomingMessage(yami::incoming_message& msg);
	virtual ~IncomingMessage() = default;

	void reply();
	void reply(Message& msg, nix::StatusCode_t status = nix::ok);
	void reply(const Job& job, nix::StatusCode_t status = nix::ok);

	void reply_with_error(nix::StatusCode_t status,
						  const std::string& msg);

	void fail(const std::string& reason = std::string());
	void fail(nix::StatusCode_t status,
			  const std::string& reason = std::string());

	void reject(const std::string& reason = std::string());
	void reject(nix::StatusCode_t status,
				const std::string& reason = std::string());
protected:
	yami::incoming_message msg_;
};


} // nix

#endif
