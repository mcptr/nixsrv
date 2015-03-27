#ifndef NIX_MODULE_API_HXX
#define NIX_MODULE_API_HXX

#include "nix/db/connection.hxx"
#include "nix/logger.hxx"
#include "nix/object_pool.hxx"

namespace nix {
namespace core {

class ModuleAPI
{
public:
	ModuleAPI(Logger& logger_ref,
			  ObjectPool<db::Connection>& db_pool_ref);

	// refs
	Logger& logger;
	ObjectPool<db::Connection>& db_pool;
};

} // core
} // nix

#endif
