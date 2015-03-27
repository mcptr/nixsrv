#include <algorithm>
#include <sstream>

#include "instance_config.hxx"


namespace nix {
namespace db {


std::string InstanceConfig::dsn(bool mask_passwd) const
{
	std::string passwd = password;
	if(mask_passwd) {
		std::transform(passwd.begin(), passwd.end(),
					   passwd.begin(), [](char)->char { return '*'; });
	}
	std::stringstream info;
	info << type << "://"
		 << "user=" << user << " "
		 << "password="  << passwd << " "
		 << "host=" << host <<  " "
		 << "port=" << port << " "
		 << "dbname=" << name;
	return info.str();
}


} // db
} // nix
