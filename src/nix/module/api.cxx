#include "api.hxx"


namespace nix {

ModuleAPI::ModuleAPI(std::shared_ptr<ObjectPool<db::Connection>> _db_pool,
					 std::shared_ptr<Logger> _logger)
	: db_pool(_db_pool),
	  logger(_logger)
{
}


} // nix
