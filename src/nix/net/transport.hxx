#ifndef NIX_TRANSPORT_HXX
#define NIX_TRANSPORT_HXX

#include <string>
#include <vector>
#include <memory>

#include "transport/options.hxx"
#include "nix/module/instance.hxx"
#include "nix/routing/route.hxx"


namespace nix {

// fwd
//class ModuleInstance;

namespace net {


class Transport
{
public:
	typedef enum { YAMI } TransportType_t;
	typedef std::vector<std::shared_ptr<const Route>> Routes_t;

	Transport() = delete;
	explicit Transport(const transport::Options& options);
	virtual ~Transport() = default;

	virtual void start() = 0;
	virtual void stop();

	virtual void register_module(std::shared_ptr<const Module> mod) = 0;

protected:
	const transport::Options& options_;
	std::string address_;
	Routes_t routing_;
};


Transport* create_transport(Transport::TransportType_t type,
							const transport::Options& options);


} // nix

#endif
