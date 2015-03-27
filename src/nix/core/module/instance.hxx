#ifndef NIX_CORE_MODULE_INSTANCE_HXX
#define NIX_CORE_MODULE_INSTANCE_HXX

#include <memory>
#include <string>

#include "api.hxx"

namespace nix {
namespace core {

// fwd
class Module;

class ModuleInstance
{
public:
	ModuleInstance() = delete;
	ModuleInstance(ModuleAPI& api,
				   const std::string& lib_path,
				   bool fatal = false);
	~ModuleInstance();
	std::shared_ptr<const Module> module() const;

	bool load(std::string& err_msg);
	bool unload(std::string& err_msg);
private:
	std::shared_ptr<Module> module_ptr_;
	std::unique_ptr<long> lib_handle_;

	ModuleAPI& api_;
	const std::string lib_path_;
	bool fatal_;
};

} // core
} // nix

#endif
