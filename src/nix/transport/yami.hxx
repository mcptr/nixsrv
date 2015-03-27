#ifndef NIX_TRANSPORT_YAMI_HXX
#define NIX_TRANSPORT_YAMI_HXX

#include <string>
#include <yami4-cpp/yami.h>

#include "nix/transport.hxx"
#include "nix/request.hxx"

#include "options.hxx"


namespace nix {


// fwd
class Module;
class ModuleInstance;
class Response;
class Route;


namespace transport {

class YAMIRequest : nix::Request<yami::incoming_message>
{
public:
	YAMIRequest() = delete;
	YAMIRequest(yami::incoming_message& im);
	void reply(nix::Response& response);
};


class YAMIRequestDispatcher
{
public:
	YAMIRequestDispatcher() = default;

	void operator()(yami::incoming_message& im);

	void set_routing(
		const std::string& module_name,
		const Transport<yami::incoming_message>::Routes_t& routing
	);

private:
	std::map<const std::string,
			 const Transport<yami::incoming_message>::Routes_t&> routing_;
};


class YAMI : public Transport<yami::incoming_message>
{
public:
	YAMI() = delete;
	explicit YAMI(const Options& options);
	virtual ~YAMI();

	void start();

	void register_module(std::shared_ptr<const Module> inst);

	virtual void register_object(
		const std::string& name, 
		std::function<void(yami::incoming_message&)> msg);

private:
	std::unique_ptr<yami::agent> agent_;
	YAMIRequestDispatcher req_dispatcher_;
	std::string build_address() const;
};


} // transport
} // nix

#endif
