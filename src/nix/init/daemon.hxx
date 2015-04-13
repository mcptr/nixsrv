#ifndef NIX_INIT_DAEMON_HXX
#define NIX_INIT_DAEMON_HXX

#include <string>


#include "nix/program_options.hxx"

// util
#include "nix/util/fs.hxx"
#include "nix/util/pid.hxx"
#include "nix/util/string.hxx"


namespace nix {
namespace init {


pid_t start_daemon(const nix::ProgramOptions& po,
				   const std::string& pidfile_path);

void stop_daemon(pid_t server_pid, const std::string& pidfile_path);


} // init
} // nix

#endif
