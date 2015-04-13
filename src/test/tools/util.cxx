#include <nix/common.hxx>
#include "util.hxx"


#if defined(EASYLOGGINGPP_H)
#undef ELPP_STACKTRACE_ON_CRASH

INITIALIZE_EASYLOGGINGPP

namespace test {

void crash_handler(int sig)
 {
	LOG(ERROR) << "Crashed";
	el::Helpers::logCrashReason(sig, true);
	el::Helpers::crashAbort(sig);
}

void initialize_test_env()
{
	el::Configurations conf;

	conf.setGlobally(
		el::ConfigurationType::ToStandardOutput,
		"true");

	conf.setGlobally(
		el::ConfigurationType::Enabled,
		"true");

	el::Helpers::setCrashHandler(crash_handler);
	
	el::Loggers::reconfigureAllLoggers(conf);

};

} // test

#else

namespace test {
void crash_handler(int sig)
{
}


void initialize_test_env()
{
}
} // namespace test


#endif


