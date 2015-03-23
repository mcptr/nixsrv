#ifndef NIX_CORE_MODULE_HXX
#define NIX_CORE_MODULE_HXX

#include <memory>
#include <functional>
#include <string>

#include "module/api.hxx"

namespace nix {
namespace core {

using std::placeholders::_1;

class Module
{
public:
	typedef std::unique_ptr<Module, std::function<void(Module*)>> ModulePointer_t;
	typedef ModulePointer_t create_t(ModuleAPI&);
	Module() = delete;
	explicit Module(ModuleAPI& api, const std::string& id, int version = 0);
	virtual ~Module() = default;

	virtual const std::string& get_ident() const;
	virtual int get_version() const;

	static void deleter(Module* ptr);

	//virtual void register_routing(Transport& t, );
	// virtual void on_incoming_message(const IncomingMessage& msg);
	// virtual void on_outgoing_message(const OutgoingMessage& msg);

protected:
	ModuleAPI& api_;
private:
	std::string ident_;
	int version_;
};


} // core
} // nix

#endif
