#ifndef NIX_CORE_CLIENT_POOL_HXX
#define NIX_CORE_CLIENT_POOL_HXX

#include "service_client.hxx"
#include "nix/object_pool.hxx"

#include <map>
#include <string>
#include <memory>


namespace nix {
namespace core {


// forwards
class Client;
class ResolverClient;
class CacheClient;
class JobQueueClient;


class ClientPool
{
public:
	ClientPool() = delete;
	explicit ClientPool(const ClientPool&) = delete;
	explicit ClientPool(const ClientConfig& config);
	virtual ~ClientPool() = default;

	std::shared_ptr<Client> generic();
	std::shared_ptr<ResolverClient> resolver();
	std::shared_ptr<CacheClient> cache();
	std::shared_ptr<JobQueueClient> job_queue();

protected:
	const ClientConfig client_config_;

	nix::ObjectPool<Client> generic_pool_;
	nix::ObjectPool<ResolverClient> resolver_pool_;
	nix::ObjectPool<CacheClient> cache_pool_;
	nix::ObjectPool<JobQueueClient> job_queue_pool_;

	void initialize_clients();

	template <class T>
	std::shared_ptr<T> acquire(nix::ObjectPool<T>& pool)
	{
		std::shared_ptr<T> ptr;
		pool.acquire(ptr);
		return ptr;
	}
};


} // core
} // nix


#endif
