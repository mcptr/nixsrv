#ifndef NIX_CORE_LOGGER_HXX
#define NIX_CORE_LOGGER_HXX

#include <memory>
#include <fstream>
#include <iostream>
#include <mutex>
#include "program_options.hxx"

namespace nix {
namespace core {

class Logger
{
public:
	enum Level { LOG_LVL_ERROR, LOG_LVL_INFO, LOG_LVL_WARN, LOG_LVL_DEBUG };
	Logger();
	explicit Logger(const ProgramOptions& options);
	~Logger();

	template <class T>
	Logger& log(Level lvl, const T& msg, bool do_flush = true);

	template <class T>
	void log_error(const T& msg, bool do_flush = true);

	template <class T>
	void log_info(const T& msg, bool do_flush = true);

	template <class T>
	void log_warn(const T& msg, bool do_flush = true);

	template <class T>
	void log_debug(const T& msg, bool do_flush = true);

	void flush();
	
	template <class T>
	Logger& operator<<(const T& msg)
	{
		*log_stream_ << msg;
		return *this;
	}

private:
	bool flushed_ = true;
	std::mutex mtx_;
	std::unique_ptr<std::ostream> log_stream_;
	std::unique_ptr<std::fstream> fstream_;
	std::string get_time();
	std::string lvl_to_str(Level lvl) const;
};

template <class T>
Logger& Logger::log(Logger::Level lvl, const T& msg, bool do_flush)
{
	mtx_.lock();
	if(!flushed_) {
		flush();
	}

	*log_stream_ << get_time() << "  [" << lvl_to_str(lvl) << "] " << msg;
	if(do_flush) {
		flush();
	}
	else {
		flushed_ = false;
	}
	mtx_.unlock();
	return *this;
}

template <class T>
void Logger::log_error(const T& msg, bool do_flush)
{
	log(Level::LOG_LVL_ERROR, msg, do_flush);
}

template <class T>
void Logger::log_info(const T& msg, bool do_flush)
{
	log(Level::LOG_LVL_INFO, msg, do_flush);
}

template <class T>
void Logger::log_warn(const T& msg, bool do_flush)
{
	log(Level::LOG_LVL_WARN, msg, do_flush);
}

template <class T>
void Logger::log_debug(const T& msg, bool do_flush)
{
	log(Level::LOG_LVL_DEBUG, msg, do_flush);
}

} // core
} // nix

#endif
