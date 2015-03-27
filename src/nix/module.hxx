#ifndef NIX_MODULE_HXX
#define NIX_MODULE_HXX

#include <memory>
#include <functional>
#include <string>

#include "nix/net/transport.hxx"
#include "nix/routing/route.hxx"

#include "module/api.hxx"


namespace nix {


using std::placeholders::_1;
using nix::net::Transport;
using nix::Route;

class Module
{
public:
	typedef std::unique_ptr<Module, std::function<void(Module*)>> ModuleInitializer_t;
	typedef ModuleInitializer_t create_t(ModuleAPI&);

	Module() = delete;
	explicit Module(ModuleAPI& api, const std::string& id, int version = 0);
	virtual ~Module() = default;

	virtual const std::string& get_ident() const;
	virtual int get_version() const;

	static void deleter(Module* ptr);

	virtual inline const Transport::Routes_t& get_routing() const final
	{
		return routes_;
	}
protected:
	ModuleAPI& api_;

private:
	const std::string ident_;
	int version_;
	Transport::Routes_t routes_;
};


} // nix

#endif
