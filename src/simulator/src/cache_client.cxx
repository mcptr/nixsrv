#include "cache_client.hxx"


namespace simulator {


void CacheClient::run(const std::string& thread_id)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(633));
	nix::Message m1;
	m1.set("key", cache_key_);
	m1.set("value.string", cache_value_);
	m1.set("value.other", u8"zażółć gęślą jaźń");
	m1.set("value.int", 123567 * client_id_);
	m1.set_array("value.array");
	m1.append("value.array", u8"πœę©ß←↓→óþ");
	m1.append("value.array", m1.to_string());
	m1.append("value.array", false);
	call("store", m1);

	nix::Message m2;
	m2.set("key", cache_key_ + std::to_string(client_id_));
	m2.set("value.thread", thread_id);
	m2.set("value.client_id", client_id_);
	call("store", m2);

	
	call("retrieve", m1);
	call("retrieve", m2);
	send_one_way("remove", m1);
	send_one_way("remove", m2);
}

void CacheClient::on_replied(const nix::Message& m)
{
	//LOG(INFO) << m.to_string();
	std::string own_key(cache_key_ + std::to_string(client_id_));
	std::string retrieved = m.get("key", "");
	if(!retrieved.empty() && retrieved.compare(own_key) == 0) {
		std::stringstream thread_id;
		thread_id << std::this_thread::get_id();
		int c_id = m.get("value.client_id", -1);
		
		if(client_id_ != c_id) {
			std::string err("Cache retrieved other thread's value");
			LOG(ERROR) << err;
			throw std::runtime_error(err);
		}
	}
}

} // simulator
