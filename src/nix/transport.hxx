#ifndef NIX_TRANSPORT_HXX
#define NIX_TRANSPORT_HXX

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "transport/options.hxx"
//#include "nix/module/instance.hxx"
//#include "nix/route.hxx"


namespace nix {


// fwd
class Module;
class ModuleInstance;
class Response;
class Route;

template<class T>
class Transport
{
public:
	typedef std::vector<std::shared_ptr<const Route>> Routes_t;

	Transport() = delete;

	explicit Transport(const transport::Options& options)
		: options_(options)
	{
	}

	virtual ~Transport() = default;

	virtual void start() = 0;

	virtual void stop() = 0;

	virtual void register_module(std::shared_ptr<const Module> mod) = 0;

	virtual void register_object(const std::string& name, 
								 std::function<void(T&)> msg) = 0;

protected:
	const transport::Options& options_;
	Routes_t routing_;
};


} // nix

#endif
