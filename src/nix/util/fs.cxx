#include <unistd.h>
#include <pwd.h>
#include <wordexp.h>
#include <errno.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <climits>


#include "fs.hxx"
#include "string.hxx"


namespace nix {
namespace util {
namespace fs {


mode_t getumask(void)
{
	mode_t mask = umask( 0 );
	umask(mask);
	return mask;
}

std::string dirname(const std::string& path)
{
	// linux... char* ...
	char cpath[PATH_MAX];
	memcpy(cpath, path.data(), path.length());
	std::string name(::dirname(cpath));
	return name;
}

std::string basename(const std::string& path)
{
	// linux... char* ...
	char cpath[PATH_MAX];
	memcpy(cpath, path.data(), path.length());
	std::string name(::basename(cpath));
	return name;
}

bool path_exists(const std::string& path)
{
	std::string resolved(expand_user(path));
	return (access(path.c_str(), F_OK) == 0);
}

bool file_exists(const std::string& path)
{
	std::string resolved(expand_user(path));
	return (access(path.c_str(), F_OK) == 0);
}

int create_dir(const std::string& path, bool recurse)
{
	std::string exp(expand_user(path));
	if(recurse) {
		std::vector<std::string> parts;
		nix::util::string::split(exp, parts, "/");
		for(auto& it : parts) {
			if(mkdir(it.c_str(), 0777 - getumask())) {
				return errno;
			}
		}
	}
	else {
		if(mkdir(path.c_str(), 0777 - getumask())) {
			return errno;
		}
	}

	return 0;
}

std::string resolve_path(const std::string& path)
{
	char buff[PATH_MAX];
	std::string exp(expand_user(path));
	realpath(exp.c_str(), buff);
	std::string out(buff);
	return out;
}

std::string expand_user(const std::string& path)
{
	if(path.length() == 0) {
		return path;
	}
	else if(path[0] != '~') {
		return path;
	}

	std::string exp;
	passwd* pw;

	std::string::size_type pos = path.find_first_of('/');
	if(path.length() == 1 || pos == 1) { // "~" || "~/"
		exp.assign(getenv("HOME"));
		if(!exp.length()) {
			pw = getpwuid(getuid());
			if(pw) {
				exp.assign(pw->pw_dir);
			}
		}
	}
	else { // "~someuser/path"
		std::string username(path, 1, (pos == std::string::npos
									   ? std::string::npos
									   : pos - 1)
		);
		pw = getpwnam(username.c_str());
		if(pw) {
			exp.assign(pw->pw_dir);
		}
	}

	if(!exp.length()) { // no subst
		return path;
	}

	if(pos == std::string::npos) { // '/' not found?
		return exp;
	}

	if(exp.length() == 0 || exp[exp.length() - 1] != '/') {
		exp += '/'; // add the slash if it's not there
	}

	exp += path.substr(pos + 1); // add everything after the slash

	return exp;
}

std::string wexpand(const std::string& path)
{
	wordexp_t exp_result;
	wordexp(path.c_str(), &exp_result, 0);
	std::string out;

	if(exp_result.we_wordc) {
		out.assign(exp_result.we_wordv[0]);
	}

	wordfree(&exp_result);

	return out;
}


} // fs
} // util
} // nix
