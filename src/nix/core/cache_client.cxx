#include "cache_client.hxx"


namespace nix {
namespace core {


CacheClient::CacheClient(const ClientConfig& config,
						 size_t max_timeout_ms_)
	: ServiceClient("Cache", config.srv_cache_address, config, max_timeout_ms_)
{
}

bool CacheClient::store(const std::string& key,
						const std::string& value)
{
	Message m;
	m.set("key", key);
	m.set("value", value);
	auto const& response = this->call("store", m);
	return response->is_status_ok();
}

bool CacheClient::store(const std::string& key,
						const Message& msg)
{
	return store(key, msg.to_string());
}

bool CacheClient::retrieve(const std::string& key, Message& result)
{
	Message m;
	m.set("key", key);
	auto const& response = this->call("retrieve", m);
	if(response->is_status_ok()) {
		result.parse(response->get(key, ""));
		return true;
	}

	return false;
}


bool CacheClient::remove(const std::string& key)
{
	Message m;
	m.set("key", key);
	return this->send_one_way("remove", m);
}

bool CacheClient::status(Message& result)
{
	Message empty;
	auto const& response = this->call("status", empty);
	if(response->is_status_ok()) {
		result = response->data();
		return true;
	}

	return false;
}


} // core
} // nix
