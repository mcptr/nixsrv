#ifndef NIX_MODULE_INSTANCE_HXX
#define NIX_MODULE_INSTANCE_HXX

#include <memory>
#include <string>

#include "api.hxx"


namespace nix {


// fwd
class Module;

class ModuleInstance
{
public:
	ModuleInstance() = delete;
	ModuleInstance(std::shared_ptr<ModuleAPI> api,
				   const std::string& lib_path,
				   bool fatal = false);
	~ModuleInstance();
	std::shared_ptr<const Module> module() const;

	bool load(std::string& err_msg);
	bool unload(std::string& err_msg);
private:
	std::shared_ptr<Module> module_ptr_;
	std::unique_ptr<long> lib_handle_;

	std::shared_ptr<ModuleAPI> api_;
	const std::string lib_path_;
	bool fatal_;
};


} // nix

#endif
