#ifndef NIX_MODULE_API_HXX
#define NIX_MODULE_API_HXX

#include "nix/db/connection.hxx"
#include "nix/logger.hxx"
#include "nix/object_pool.hxx"


namespace nix {


class ModuleAPI
{
public:
	ModuleAPI(std::shared_ptr<ObjectPool<db::Connection>> _db_pool,
			  std::shared_ptr<Logger> _logger);

	// refs
	std::shared_ptr<ObjectPool<db::Connection>> db_pool;
	std::shared_ptr<Logger> logger;
};


} // nix

#endif
