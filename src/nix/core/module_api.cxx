#include "module_api.hxx"

namespace nix {
namespace core {

Module::ModuleAPI(ObjectPool<db::Connection>& db_pool)
	: db_pool_(db_pool)
{
}

} // core
} // nix
