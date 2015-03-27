#include <fstream>

#include "options.hxx"


namespace nix {
namespace db {


void Options::parse(const std::string& config_path)
{
	using namespace std;
	string stropt;
	int intopt;

	po::options_description db_options;
	po::variables_map vm;

	db_options.add_options()
		("global.databases",
			 po::value(&stropt)->default_value(""),
			 "global.databases")
	;

	ifstream cf(config_path.c_str());
	if(!cf.good()) {
		throw nix::InitializationError("No db config found (" + config_path + ")");
	}

	po::store(po::parse_config_file(cf, db_options, true), vm);
	po::notify(vm);
	istringstream databases(vm["global.databases"].as<string>());

	std::vector<std::string> dblist;

	copy(istream_iterator<string>(databases),
		 istream_iterator<string>(),
		 back_inserter<vector<string> >(dblist));

	for(auto& entry : dblist) {
		string user_entry(entry + ".user");
		string dbname_entry(entry + ".name");
		string passwd_entry(entry + ".password");
		string host_entry(entry + ".host");
		string port_entry(entry + ".port");
		string pool_entry(entry + ".pool_size");

		db_options.add_options()
			(user_entry.c_str(),
			 po::value(&stropt)->default_value(""),
			 user_entry.c_str()
			)
			(dbname_entry.c_str(),
			 po::value(&stropt)->default_value(""),
			 dbname_entry.c_str()
			)
			(passwd_entry.c_str(),
			 po::value(&stropt)->default_value(""),
			 passwd_entry.c_str()
			)
			(host_entry.c_str(),
			 po::value(&stropt)->default_value(""),
			 host_entry.c_str()
			)
			(port_entry.c_str(),
			 po::value(&intopt)->default_value(0),
			 port_entry.c_str()
			)
			(pool_entry.c_str(),
			 po::value(&intopt)->default_value(1),
			 pool_entry.c_str())
			;
	}

	cf.clear();
	cf.seekg(0, ios::beg);

	po::store(po::parse_config_file(cf, db_options, true), vm);
	po::notify(vm);
	cf.close();

	for(auto& entry : dblist) {
		std::shared_ptr<InstanceConfig> inst(new InstanceConfig());
		inst->alias = entry;
		inst->type = get<string>(entry + ".type");
		inst->name = get<string>(entry + ".name");
		inst->user = get<string>(entry + ".user");
		inst->password = get<string>(entry + ".password");
		inst->host = get<string>(entry + ".host");
		inst->port = get<int>(entry + ".port");

		int pool_size = get<int>(entry + ".pool_size");
		inst->pool_size = (pool_size ? pool_size : 1);
		instances_.push_back(inst);
	}
}


} // db
} // nix
