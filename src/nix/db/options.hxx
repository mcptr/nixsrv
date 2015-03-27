#ifndef NIX_DB_OPTIONS_HXX
#define NIX_DB_OPTIONS_HXX

#include <memory>
#include <vector>

#include "nix/core/exception.hxx"
#include "nix/core/options.hxx"

#include "instance_config.hxx"

namespace nix {
namespace core {
namespace db {

namespace po = boost::program_options;

class Options : public nix::core::Options
{
public:
	typedef std::vector<std::shared_ptr<InstanceConfig>> InstanceList_t;

	void parse(const std::string& config_path);

	inline const InstanceList_t& get_instances() const
	{
		return instances_;
	}
private:
	InstanceList_t instances_;

};

} // db
} // core
} // nix

#endif
