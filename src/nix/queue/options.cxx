#include <fstream>

#include "options.hxx"


namespace nix {
namespace queue {


void Options::parse(const std::string& config_path)
{
	using namespace std;
	string stropt;
	int intopt;

	const int default_queue_size = 100;

	po::options_description options;

	options.add_options()
		("builtins.queues",
			 po::value(&stropt)->default_value(""),
			 "builtins.queues")
	;

	ifstream cf(config_path.c_str());
	if(!cf.good()) {
		throw nix::InitializationError("No builtin queue config file found (" + config_path + ")");
	}

	po::store(po::parse_config_file(cf, options, true), vm_);
	po::notify(vm_);
	istringstream queues(vm_["builtins.queues"].as<string>());

	std::vector<std::string> queues_list;

	copy(istream_iterator<string>(queues),
		 istream_iterator<string>(),
		 back_inserter<vector<string> >(queues_list));

	for(auto& entry : queues_list) {
		string size_entry("queue." + entry + ".size");

		options.add_options()
			(size_entry.c_str(),
			 po::value(&intopt)->default_value(default_queue_size),
			 size_entry.c_str()
			)
			;
	}

	cf.clear();
	cf.seekg(0, ios::beg);

	po::store(po::parse_config_file(cf, options, true), vm_);
	po::notify(vm_);
	cf.close();

	for(auto& entry : queues_list) {
		std::shared_ptr<InstanceConfig> inst(new InstanceConfig());
		inst->name = entry;
		int size_entry = get<int>("queue." + entry + ".size");
		inst->size = (size_entry ? size_entry : default_queue_size);
		instances_.push_back(inst);
	}
}


} // queue
} // nix
