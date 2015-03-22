#include <pwd.h>
#include <libgen.h>
#include <cstring>
#include <fstream>
#include <iostream>

#include "exception.hxx"
#include "program_options.hxx"
#include "nix/util/fs.hxx"

namespace nix {
namespace core {

void ProgramOptions::parse(int argc, char** argv)
{
	using namespace std;

	try {
		po::options_description flags("General");
		po::options_description general("Startup options");
		po::options_description server("Server options");
		po::options_description database("Database options");

		po::options_description config_file_hidden(
			"Hidden configuration options"
		);

		string progname(argv[0]);
		string base_dir(dirname(dirname((char*)progname.c_str())));
		base_dir = nix::util::fs::resolve_path(base_dir);
		string config_path;

		string stropt;
		int intopt;

		flags.add_options()
			("help,h", "display this help")
			;

		general.add_options()
			("config,c", po::value<string>(&config_path)->default_value(
				base_dir + "/etc/config.ini", "$BASE_DIR/etc/config.ini"), "")
 			("no_config,N",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "Do not read configuration file"
			)
			("basedir", po::value(&stropt)->default_value(base_dir), "base application directory")
			("pluginsdir", po::value(&stropt)->default_value(base_dir + "/lib", "$BASE_DIR/lib"), "directory containing plugins (*.so)")
			("logdir", po::value(&stropt)->default_value(base_dir + "/logs", "$BASE_DIR/logs"), "directory to store logs to")
			("pidbase", po::value(&stropt)->default_value(base_dir, "$BASE_DIR"), "directory for storing pidfile")
			("pidname", po::value(&stropt)->default_value(base_dir + "/daemon.pid", "$BASE_DIR/$(pidbase)/daemon.pid"), "pid filename")
			("verbose,v",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "verbose run"
			)
			("fatal,F",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "make all errors fatal"
			)
			("foreground,f",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "run in foreground"
			)
			("debug",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "debug mode (additional info will be printed)"
			)
			;

		server.add_options()
			(
				"server_af,A", po::value<string>(),
				"address family (tcp, unix)"
			)
			(
				"server_address,L", po::value<string>(),
				"address to listen on"
			)
			(
				"server_port,p", po::value<int>(), "port to use"
			)
			(
				"server_threads,t", po::value<int>(),
				"dispatcher threads"
			)
			;

		passwd* pwd = getpwuid(getuid());
		string username(pwd->pw_name);

		database.add_options()
			(
				"dbpool_size", po::value<int>()->composing()->default_value(0),
				"database pool size (default from config)"
			)
			(
				"dbtype",
				po::value<string>()->composing()->default_value(""),
				"database engine"
			)
			// (
			//		"dbname,D", po::value<string>()->composing(),
			//		"database name"
			// )
			// (
			//		"dbuser,U", po::value<string>()->composing()->default_value(username),
			//		"database username"
			// )
			// (
			//		"dbpass,P", po::value<string>()->composing(),
			//		"password"
			// )
			;

		config_file_hidden.add_options()
			("NET.server_af", po::value(&stropt)->default_value("tcp"), "NET.server_af")
			("NET.server_address", po::value(&stropt)->default_value("127.0.0.1"), "NET.server_address")
			("NET.server_port", po::value(&intopt)->default_value(9876), "NET.server_port")
			("NET-YAMI.tcp_listen_backlog", po::value(&intopt)->default_value(10), "NET-YAMI.tcp_listen_backlog")
			("NET-YAMI.tcp_nonblocking", po::value(&intopt)->default_value(1), "NET-YAMI.tcp_nonblocking")
			("NET-YAMI.dispatcher_threads", po::value(&intopt)->default_value(10), "NET-YAMI.dispatcher_threads")
			;

		all_.add(flags).add(general).add(server).add(database);

		po::store(po::command_line_parser(argc, argv).options(all_).run(), vm_);
		po::notify(vm_);

		if(!has_help()) {
			config_file_hidden.add(general).add(server).add(database);
			ifstream cf(config_path.c_str());
			if(cf.good()) {
				po::store(po::parse_config_file(cf, config_file_hidden, true), vm_);
				po::notify(vm_);
				cf.close();
			}
			else {
				if(!vm_["no_config"].as<bool>()) {
					throw nix::core::InitializationError("Configuration file not found: " + config_path);
				}
			}
		}
	}
	catch(exception& e) {
		cerr << "error: " << e.what() << "\n";
		throw nix::core::InitializationError(e.what());
	}
}

bool ProgramOptions::has_help() const
{
	return vm_.count("help");
}

void ProgramOptions::display_help() const
{
	std::cout << all_ << std::endl;
}

} // core
} // nix
