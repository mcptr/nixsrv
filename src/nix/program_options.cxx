#include <pwd.h>
#include <libgen.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <climits>
#include <sys/utsname.h>

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
		po::options_description builtins_hidden("Configuration of builtins modules");
		po::options_description infrastructure("Infrastructure options");
		po::options_description resources("Resources");
		po::options_description auth("Auth");
		po::options_description devel("Development options");

		po::options_description config_file_hidden(
			"Configuration file based options"
		);

		string progname(argv[0]);
		char progn[PATH_MAX];
		memcpy(progn, progname.data(), progname.length());
		string base_dir(dirname(dirname(progn)));
		base_dir = nix::util::fs::expand_user(base_dir);
		string config_path;
		string db_config_path;

		struct utsname utsn;
		int status = uname(&utsn);
		if(status != 0) {
			throw std::runtime_error("Unable to read host name");
		}

		std::string nodename(utsn.nodename);

		string stropt;
		int intopt;
		bool boolopt;

		flags.add_options()
			("help,h", "display this help")
			;

		generic.add_options()
			("config,c",
			 po::value<string>(&config_path)->default_value(
				 base_dir + "/etc/config.ini", "$BASE_DIR/etc/config.ini"
			 ),
			 "configuration path")
			("no_config,N",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "Do not read configuration file"
			)
			("nodename",
			 po::value<string>(&stropt)->default_value(nodename),
			 "set node name to be used accross infrastracture"
			)
			("dbconfig",
			 po::value<string>(&db_config_path)->default_value(
				 base_dir + "/etc/db.ini", "$BASE_DIR/etc/db.ini"), ""
			)
			("basedir", po::value(&stropt)->default_value(base_dir),
			 "base application directory"
			)
			("modulesdir",
			 po::value(&stropt)->default_value(base_dir + "/lib", "$BASE_DIR/lib"),
			 "directory containing modules (*.so)"
			)
			("logdir",
			 po::value(&stropt)->default_value(base_dir + "/logs", "$BASE_DIR/logs"),
			 "directory to store logs to"
			)
			("pidfile",
			 po::value(&stropt)->default_value(base_dir + "/nix.pid", "$BASE_DIR"),
			 "pidfile path"
			)
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
			("no-close-fds",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "do not close stdout/stderr when daemonizing"
			)
			;

		server.add_options()
			("address,A", po::value(&stropt)->default_value("tcp://*:*"),
			 "address to listen on")
			("threads,t", po::value(&intopt)->default_value(0),
			 "dispatcher threads (overwrites config value)")
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

		builtins_hidden.add_options()
			// module manager
			("module-manager.manager_thread_enabled",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "enable internal module manager thread")
			("module-manager.manager_thread_run_interval",
			 po::value(&intopt)->default_value(60),
			 "manager wakup every run * sleep interval")
			("module-manager.manager_thread_sleep_interval_ms",
			 po::value(&intopt)->default_value(1000),
			 "manager thread sleep interval")

			// cache cleaner
			("builtins.cache.cleaner_enabled",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "enable cleaner thread")
			("builtins.cache.cleaner_run_interval",
			 po::value(&intopt)->default_value(60),
			 "cleaner run (run * sleep)")
			("builtins.cache.cleaner_sleep_interval_ms",
			 po::value(&intopt)->default_value(1000),
			 "cleaner sleep interval in ms (run * sleep)")

			// resolver monitor
			("builtins.resolver.monitor_enabled",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "enable monitor thread in resolver")
			("builtins.resolver.monitor_run_interval",
			 po::value(&intopt)->default_value(20),
			 "monitor run (run * sleep)")
			("builtins.resolver.monitor_sleep_interval_ms",
			 po::value(&intopt)->default_value(1000),
			 "resolver monitor sleep interval in ms (run * sleep)")
			("builtins.resolver.monitor_response_timeout_ms",
			 po::value(&intopt)->default_value(2000),
			 "how long to wait for response when checking if service is alive")
			("builtins.resolver.monitor_max_failures",
			 po::value(&intopt)->default_value(3),
			 "monitor will remove bound addresses after max_failures reached")
		
			;

		infrastructure.add_options()
			("infrastructure.srv_resolver_address",
			 po::value(&stropt)->default_value(""),
			 "external resolver address")
			("infrastructure.srv_cache_address",
			 po::value(&stropt)->default_value(""),
			 "external cache server address")
			("infrastructure.srv_broker_address",
			 po::value(&stropt)->default_value(""),
			 "external broker address")
			("infrastructure.srv_job_queue_address",
			 po::value(&stropt)->default_value(""),
			 "external job queue server address")
			;

		resources.add_options()
			("resources.generic_client_pool_size",
			 po::value<int>(&intopt)->default_value(1),
			 ""
			)
			("resources.resolver_client_pool_size",
			 po::value<int>(&intopt)->default_value(1),
			 ""
			)
			("resources.cache_client_pool_size",
			 po::value<int>(&intopt)->default_value(1),
			 ""
			)
			("resources.job_queue_client_pool_size",
			 po::value<int>(&intopt)->default_value(1),
			 ""
			)
			("resources.broker_client_pool_size",
			 po::value<int>(&intopt)->default_value(1),
			 ""
			)
			;

		auth.add_options()
			("auth.api_key_private",
			 po::value<string>(&stropt)->default_value(""),
			 "api auth key for querying private services"
			)
			("auth.api_key_public",
			 po::value<string>(&stropt)->default_value(""),
			 "api auth key for querying public services")
			;

		devel.add_options()
			("development-mode",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "enable development mode"
			)
			("enable-random-sleep",
			 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
			 "enable random sleep in builtin modules threads\n(only in 'DEBUG_BUILD' and when development mode enabled)"
			)
			;

		passwd* pwd = getpwuid(getuid());
		string username(pwd->pw_name);

		all_.add(flags).add(generic).add(server).add(builtins).add(devel);
		po::store(po::command_line_parser(argc, argv).options(all_).run(), vm_);
		po::notify(vm_);


		config_file_hidden.add_options()
			("server.tcp_listen_backlog",
			 po::value(&intopt)->default_value(10), "server.tcp_listen_backlog"
			)
			("server.tcp_nonblocking",
			 po::value(&boolopt)->default_value(true), "server.tcp_nonblocking"
			)
			("server.dispatcher_threads",
			 po::value(&intopt)->default_value(10), "server.dispatcher_threads"
			)
			;

		if(!has_help()) {
			config_file_hidden.add(generic).add(server).add(builtins)
				.add(infrastructure).add(resources)
				.add(builtins_hidden).add(auth);

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
