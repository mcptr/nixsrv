#ifndef NIX_REQUEST_HXX
#define NIX_REQUEST_HXX

#include <memory>
#include <string>
#include "message/incoming.hxx"
#include "message/outgoing.hxx"
#include "response.hxx"


namespace nix {


template<class TransportMessage_t>
class Request
{
public:
	Request() = delete;
	explicit Request(TransportMessage_t& inc_transport_msg)
		: im_(inc_transport_msg)
	{
		seq_id_++;
		message_.reset(new IncomingMessage());
	}

	virtual ~Request() = default;

	virtual inline int get_seq_id() const final
	{
		return seq_id_;
	}

	virtual inline const std::string& get_module() const final
	{
		return module_;
	}

	virtual inline const std::string& get_route() const final
	{
		return route_;
	}

	virtual const IncomingMessage& get_message() const
	{
		return *(message_.get());
	}

	virtual void reply(Response& response) = 0;

protected:
	const TransportMessage_t& im_;
	std::shared_ptr<IncomingMessage> message_;

	std::string module_;
	std::string route_;

private:
	static int seq_id_;
};

template<class TransportMessage_t>
int Request<TransportMessage_t>::seq_id_ = 0;


} // nix

#endif
