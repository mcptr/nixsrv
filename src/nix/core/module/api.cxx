#include "api.hxx"

namespace nix {
namespace core {

ModuleAPI::ModuleAPI(ObjectPool<db::Connection>& db_pool)
	: db_pool_(db_pool)
{
}



} // core
} // nix
