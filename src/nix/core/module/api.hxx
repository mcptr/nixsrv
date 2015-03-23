#ifndef NIX_CORE_MODULE_API_HXX
#define NIX_CORE_MODULE_API_HXX

#include "nix/core/db/connection.hxx"
#include "nix/core/object_pool.hxx"

namespace nix {
namespace core {

class ModuleAPI
{
public:
	ModuleAPI(ObjectPool<db::Connection>& db_pool);

	// refs
	ObjectPool<db::Connection>& db_pool_;
};

} // core
} // nix

#endif
