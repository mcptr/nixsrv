#ifndef NIX_MODULE_HXX
#define NIX_MODULE_HXX

#include <memory>
#include <functional>
#include <string>
#include <vector>

#include "nix/route.hxx"
#include "module/api.hxx"


namespace nix {


using std::placeholders::_1;

class Module
{
public:
	typedef std::unique_ptr<Module, std::function<void(Module*)>> ModuleInitializer_t;
	typedef ModuleInitializer_t create_t(ModuleAPI&);
	typedef std::vector<std::shared_ptr<Route>> Routes_t;

	Module() = delete;
	explicit Module(std::shared_ptr<ModuleAPI> api, const std::string& id, int version = 0);
	virtual ~Module();

	virtual const std::string& get_ident() const final;
	virtual int get_version() const final;

	static void deleter(Module* ptr);

	virtual inline const Routes_t& get_routing() const final
	{
		return routes_;
	}
protected:
	std::shared_ptr<ModuleAPI> api_;
	Routes_t routes_;

private:
	const std::string ident_;
	int version_;
};


} // nix

#endif
