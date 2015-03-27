#ifndef NIX_PROGRAM_OPTIONS_HXX
#define NIX_PROGRAM_OPTIONS_HXX

#include "options.hxx"


namespace nix {


namespace po = boost::program_options;

class ProgramOptions : public Options
{
public:
	void parse(int argc, char** argv);
	bool has_help() const;
	void display_help() const;

	inline bool is_verbose() const
	{
		return (vm_["debug"].as<bool>() || vm_["verbose"].as<bool>());
	}

private:
	void dump_variables() const;
	po::options_description all_;
};


} // nix

#endif
