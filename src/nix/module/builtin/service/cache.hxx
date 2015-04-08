#ifndef NIX_MODULE_BUILTIN_SERVICE_CACHE_HXX
#define NIX_MODULE_BUILTIN_SERVICE_CACHE_HXX

#include <memory>
#include <unordered_map>
#include "nix/module.hxx"
#include "nix/message/incoming.hxx"


namespace nix {
namespace module {


class CacheEntry
{
public:
	CacheEntry();
	bool expired() const;

	int ctime;
	// FIXME: read this from config for builtins
	int validity_period = 15 * 60; // seconds
	std::string content;
};

class Cache : public Module
{
public:
	Cache() = delete;
	virtual ~Cache() = default;
	explicit Cache(std::shared_ptr<ModuleAPI> api);

private:
	void store(std::unique_ptr<IncomingMessage> msg);
	void retrieve(std::unique_ptr<IncomingMessage> msg);
	void remove(std::unique_ptr<IncomingMessage> msg);

	std::unordered_map<std::string, CacheEntry> cache_;
};


} // module
} // nix

#endif
