#include <pwd.h>
#include <libgen.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <climits>

#include "exception.hxx"
#include "program_options.hxx"
#include "nix/util/fs.hxx"


namespace nix {


void ProgramOptions::parse(int argc, char** argv)
{
	using namespace std;

	try {
		po::options_description flags("Generic");
		po::options_description generic("Startup options");
		po::options_description server("Server options");
		po::options_description builtins("Builtins");
		po::options_description infrastructure("Infrastructure options");

		po::options_description config_file_hidden(
			"Configuration file based options"
		);

		string progname(argv[0]);
		char progn[PATH_MAX];
		memcpy(progn, progname.data(), progname.length());
		string base_dir(dirname(dirname(progn)));
		base_dir = nix::util::fs::resolve_path(base_dir);
		string config_path;
		string db_config_path;

		string stropt;
		int intopt;
		bool boolopt;

		flags.add_options()
			("help,h", "display this help")
			;

		generic.add_options()
			("config,c", po::value<string>(&config_path)->default_value(
				base_dir + "/etc/config.ini", "$BASE_DIR/etc/config.ini"), "")
			("no_config,N",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "Do not read configuration file"
			)
			("dbconfig", po::value<string>(&db_config_path)->default_value(
				base_dir + "/etc/config.ini", "$BASE_DIR/etc/db.ini"), "")
			("basedir", po::value(&stropt)->default_value(base_dir), "base application directory")
			("modulesdir", po::value(&stropt)->default_value(base_dir + "/lib", "$BASE_DIR/lib"), "directory containing modules (*.so)")
			("logdir", po::value(&stropt)->default_value(base_dir + "/logs", "$BASE_DIR/logs"), "directory to store logs to")
			("pidfile", po::value(&stropt)->default_value(base_dir + "/nix.pid", "$BASE_DIR"), "pidfile path")
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
			("debug,D",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "debug mode (additional info will be printed)"
			)
			;

		server.add_options()
			("address,A", po::value(&stropt)->default_value("tcp://*:*"),
			 "address to listen on")
			;

		builtins.add_options()
			("enable-debug",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "enable debug module")
			("enable-cache",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "enable cache module")
			("enable-resolver",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "enable resolver module")
			("enable-job-queue",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "enable job queue module")
			;

		infrastructure.add_options()
			("srv_resolver_address",
			 po::value(&stropt)->default_value(""),
			 "external resolver address")
			("srv_cache_address",
			 po::value(&stropt)->default_value(""),
			 "external cache server address")
			("srv_broker_address",
			 po::value(&stropt)->default_value(""),
			 "external broker address")
			("srv_job_queue_address",
			 po::value(&stropt)->default_value(""),
			 "external job queue server address")
			;

		passwd* pwd = getpwuid(getuid());
		string username(pwd->pw_name);

		config_file_hidden.add_options()
			("SERVER.tcp_listen_backlog",
			 po::value(&intopt)->default_value(10), "SERVER.tcp_listen_backlog"
			)
			("SERVER.tcp_nonblocking",
			 po::value(&boolopt)->default_value(true), "SERVER.tcp_nonblocking"
			)
			("SERVER.dispatcher_threads",
			 po::value(&intopt)->default_value(10), "SERVER.dispatcher_threads"
			)
			;

		all_.add(flags).add(generic).add(server).add(builtins);

		po::store(po::command_line_parser(argc, argv).options(all_).run(), vm_);
		po::notify(vm_);

		if(!has_help()) {
			config_file_hidden.add(generic).add(server).add(builtins).add(infrastructure);
			if(vm_["debug"].as<bool>()) {
				std::cout << "Reading config file: " << config_path << std::endl;
			}

			ifstream cf(config_path.c_str());

			if(cf.good()) {
				po::store(po::parse_config_file(cf, config_file_hidden, true), vm_);
				po::notify(vm_);
			}
			else {
				if(!vm_["no_config"].as<bool>()) {
					throw nix::InitializationError("Configuration file not found: " + config_path);
				}
			}

			cf.close();
		}
	}
	catch(exception& e) {
		cerr << "error: " << e.what() << "\n";
		throw nix::InitializationError(e.what());
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


} // nix
