#ifndef NIX_OPTIONS_HXX
#define NIX_OPTIONS_HXX

#include <boost/program_options.hpp>
#include <string>


namespace nix {


namespace po = boost::program_options;

class Options
{
public:
	Options() = default;
	virtual ~Options() = default;

	template <class T>
	const T get(const std::string& k) const
	{
		try {
			return vm_[k].as<T>();
		}
		catch(boost::bad_any_cast& e) {
			std::cerr << "ERROR: Options::get(): " << k << std::endl;
			throw e;
		}
	}

	void dump_variables_map() const;

protected:
	po::variables_map vm_;
};


} // nix

#endif
