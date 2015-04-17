#include "client_pool.hxx"

#include "resolver_client.hxx"
#include "cache_client.hxx"
#include "job_queue_client.hxx"


namespace nix {
namespace core {


ClientPool::ClientPool(const ClientConfig& config)
	: client_config_(config)
{
	initialize_clients();
}

std::shared_ptr<Client>
ClientPool::generic()
{
	return acquire<Client>(generic_pool_);
}

std::shared_ptr<ResolverClient>
ClientPool::resolver()
{
	return acquire<ResolverClient>(resolver_pool_);
}

std::shared_ptr<CacheClient>
ClientPool::cache()
{
	return acquire<CacheClient>(cache_pool_);
}

std::shared_ptr<JobQueueClient>
ClientPool::job_queue()
{
	return acquire<JobQueueClient>(job_queue_pool_);
}

void ClientPool::initialize_clients()
{
	size_t size = client_config_.generic_client_pool_size;
	for(size_t i = 0; i < size; i++) {
		generic_pool_.insert(new Client(client_config_));
	}

	size = client_config_.resolver_client_pool_size;
	for(size_t i = 0; i < size; i++) {
		resolver_pool_.insert(new ResolverClient(client_config_));
	}

	size = client_config_.cache_client_pool_size;
	for(size_t i = 0; i < size; i++) {
		cache_pool_.insert(new CacheClient(client_config_));
	}

	size = client_config_.job_queue_client_pool_size;
	for(size_t i = 0; i < size; i++) {
		job_queue_pool_.insert(new JobQueueClient(client_config_));
	}
}


} // core
} // nix
