#include "api.hxx"


namespace nix {

ModuleAPI::ModuleAPI(std::shared_ptr<ObjectPool<db::Connection>> _db_pool)
	: db_pool(_db_pool)
{
}


} // nix
