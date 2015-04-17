#include "api.hxx"


namespace nix {

ModuleAPI::ModuleAPI(std::shared_ptr<ObjectPool<db::Connection>> _db_pool,
					 std::shared_ptr<core::ClientPool> _client_pool)
	: db_pool(_db_pool),
	  client_pool(_client_pool)
{
}


} // nix
