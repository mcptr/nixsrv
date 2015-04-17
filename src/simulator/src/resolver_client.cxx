#include "resolver_client.hxx"


namespace simulator {


void ResolverClient::run(const std::string& thread_id)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(333));
	nix::Message m;
	m.set("@api_key", api_key_);
	m.set("nodename", "simulator-01");
	m.set("address", "tcp://127.0.0.1:12345");
	call("bind", m);
	call("resolve", m);
	call("unbind", m);
}

// void ResolverClient::on_replied(const nix::Message& m)
// {
// 	std::string retrieved = m.get("value.thread", "");
// 	if(!retrieved.empty()) {
// 		std::stringstream thread_id;
// 		thread_id << std::this_thread::get_id();
// 		if(retrieved.compare(thread_id.str()) != 0) {
// 			std::string err("Cache retrieved other thread's value");
// 			LOG(ERROR) << err;
// 			throw std::runtime_error(err);
// 		}
// 	}
// }


} // simulator
