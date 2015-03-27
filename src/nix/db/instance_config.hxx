#ifndef NIX_DB_INSTANCE_CONFIG_HXX
#define NIX_DB_INSTANCE_CONFIG_HXX

#include <string>


namespace nix {
namespace db {


class InstanceConfig
{
public:
	std::string alias;
	std::string type;
	std::string user;
	std::string password;
	std::string host;
	std::string name;
	unsigned port;
	size_t pool_size;
	
	std::string dsn(bool mask_passwd = false) const;
};


} // db
} // nix

#endif
