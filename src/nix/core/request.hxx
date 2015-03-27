#ifndef NIX_CORE_REQUEST_HXX
#define NIX_CORE_REQUEST_HXX

#include <memory>
#include <string>
#include "nix/api/message/incoming.hxx"
#include "nix/api/message/outgoing.hxx"
#include "nix/api/response.hxx"

namespace nix {
namespace core {


template<class TransportMessage_t>
class Request
{
public:
	Request() = delete;
	explicit Request(TransportMessage_t& inc_transport_msg);
	virtual ~Request() = default;

	virtual int get_seq_id() const final;

	virtual inline const std::string& get_module() const final
	{
		return module_;
	}

	virtual inline const std::string& get_route() const final
	{
		return route_;
	}

	virtual const api::IncomingMessage& get_message() const;
protected:
	const TransportMessage_t& im_;
	std::shared_ptr<api::IncomingMessage> message_;
	std::shared_ptr<api::Response> response_;

	std::string module_;
	std::string route_;
private:
	static int seq_id_;
};

template<class TransportMessage_t>
int Request<TransportMessage_t>::seq_id_ = 0;

template<class TransportMessage_t>
Request<TransportMessage_t>::Request(TransportMessage_t& inc_transport_msg)
	: im_(inc_transport_msg)
{
	seq_id_++;
	message_.reset(new api::IncomingMessage());
}

template<class TransportMessage_t>
int Request<TransportMessage_t>::get_seq_id() const
{
	return seq_id_;
}

template<class TransportMessage_t>
const api::IncomingMessage&
Request<TransportMessage_t>::get_message() const
{
	return *(message_.get());
}


} // core
} // nix

#endif
