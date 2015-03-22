#include <iomanip>
#include <ctime>

#include "logger.hxx"
#include "exception.hxx"
#include "nix/util/fs.hxx"

namespace nix {
namespace core {

Logger::Logger()
{
	log_stream_.reset(new std::ostream(std::cerr.rdbuf()));
}

Logger::Logger(const ProgramOptions& options)
{
	std::streambuf* buf;

	if(options.get<bool>("foreground")) {
		log_stream_.reset(&std::cerr);
		//buf = std::cerr.rdbuf();
	}
	else {
		std::string lf(
			nix::util::fs::resolve_path(
				options.get<std::string>("logdir")
			)
		);

		if(!nix::util::fs::path_exists(lf)) {
			throw InitializationError("Invalid log directory: " + lf);
		}
		lf.append("/error.log");
		log_stream_.reset(new std::fstream(lf.c_str(), std::ios::app));
		//fstream_.reset(new std::fstream(lf.c_str(), std::ios::app));
		//buf = fstream_->rdbuf();
	}
};

Logger::~Logger()
{
	if(fstream_) {
		fstream_->close();
		fstream_.reset();
	}
	log_stream_.reset();
}

void Logger::flush()
{
	*log_stream_ << std::endl;
	flushed_ = true;
}

std::string Logger::get_time() {
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	std::string t(asctime(timeinfo));
	return t.substr(0, (t.length() - 1));
}

std::string Logger::lvl_to_str(Level lvl) const
{
	std::string out;
	switch(lvl) {
	case LOG_LVL_ERROR:
		out = "ERROR";
		break;
	case LOG_LVL_INFO:
		out = "INFO";
		break;
	case LOG_LVL_WARN:
		out = "WARN";
		break;
	case LOG_LVL_DEBUG:
		out = "DEBUG";
		break;
	}
	return out;
}

} // core
} // nix
