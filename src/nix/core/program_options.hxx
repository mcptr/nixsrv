#ifndef NIX_CORE_PROGRAM_OPTIONS_HXX
#define NIX_CORE_PROGRAM_OPTIONS_HXX

#include <boost/program_options.hpp>
#include <string>

namespace nix {
namespace core {

namespace po = boost::program_options;

class ProgramOptions
{
public:
	void parse(int argc, char** argv);
	bool has_help() const;
	void display_help() const;

	template <class T>
	const T get(const std::string& k) const
	{
		try {
			return vm_[k].as<T>();
		}
		catch(boost::bad_any_cast& e) {
			std::cerr << "ERROR: ProgramOptions::get(): " << k << std::endl;
			throw e;
		}
	}
private:
	po::variables_map vm_;
	po::options_description all_;
};

} // core
} // nix

#endif
