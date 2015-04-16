#ifndef NIX_MODULE_API_HXX
#define NIX_MODULE_API_HXX

#include "nix/object_pool.hxx"
#include "nix/core/client_pool.hxx"
#include "nix/db/connection.hxx"

namespace nix {


class ModuleAPI
{
public:
	ModuleAPI(std::shared_ptr<ObjectPool<db::Connection>> _db_pool,
			  std::shared_ptr<core::ClientPool> _client_pool);

	const std::shared_ptr<core::ClientPool> client_pool;
	const std::shared_ptr<ObjectPool<db::Connection>> db_pool;
};


} // nix

#endif
