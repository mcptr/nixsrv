#include "resolver_client.hxx"


namespace simulator {


void ResolverClient::run()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(333));
	nix::Message m;
	m.set("@api_key", api_key_);
	m.set("nodename", "simulator-01");
	m.set("address", "tcp://127.0.0.1:12345");
	LOG(DEBUG) << "ResolverClient bind";
	call("Resolver", "bind", m);
	LOG(DEBUG) << "ResolverClient resolve";
	call("Resolver", "resolve", m);
	LOG(DEBUG) << "ResolverClient unbind";
	call("Resolver", "unbind", m);
}


} // simulator
