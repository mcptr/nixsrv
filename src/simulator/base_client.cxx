#include "base_client.hxx"


namespace simulator {


BaseClient::BaseClient(const std::string& address)
	: server_address_(address)
{
}

void BaseClient::call(const std::string& module,
					  const std::string& route,
					  const nix::Message& msg,
					  int timeout)
{
        yami::parameters params;
        params.set_string("message", msg.to_string());

        std::auto_ptr<yami::outgoing_message> om(
            agent_.send(
				server_address_,
				module, route, params));

		if(timeout) {
			om->wait_for_completion(timeout);
		}
		else if(timeout < 0) {
			om->wait_for_completion();
		}

        const yami::message_state state = om->get_state();

		if(state == yami::replied) {
			const yami::parameters& reply = om->get_reply();
			nix::Message m(reply.get_string("message"));
			this->on_replied(m);
		}
		else if(state == yami::rejected) {
			this->on_rejected(om->get_exception_msg());
		}
		else {
			this->on_abanoned();
		}
}

void BaseClient::on_replied(nix::Message& m)
{
}

void BaseClient::on_rejected(const std::string& reason)
{
}

void BaseClient::on_abanoned()
{
}


} // simulator

