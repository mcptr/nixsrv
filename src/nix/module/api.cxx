#include "api.hxx"


namespace nix {


ModuleAPI::ModuleAPI(Logger& logger_ref,
					 ObjectPool<db::Connection>& db_pool_ref)
	: logger(logger_ref),
	  db_pool(db_pool_ref)
{
}


} // nix
