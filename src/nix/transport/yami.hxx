#ifndef NIX_TRANSPORT_YAMI_HXX
#define NIX_TRANSPORT_YAMI_HXX

#include <string>
#include <yami4-cpp/yami.h>
#include <unordered_map>

#include "nix/transport.hxx"
#include "nix/request.hxx"
#include "nix/response.hxx"

#include "options.hxx"


namespace nix {


// fwd
class Module;
class ModuleInstance;
class Route;

namespace transport {


class YAMIRequest : public nix::Request<yami::incoming_message>
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

	// first: "module::route"
	typedef std::map<const std::string, std::shared_ptr<const Route>> Routing_t;

private:
	Routing_t routing_;
};


class YAMI : public Transport<yami::incoming_message>
{
public:
	typedef std::function<void(yami::incoming_message&)> DirectHandler_t;
	typedef std::function<void(int, const char*)> IOErrorHandler_t;

	YAMI() = delete;
	explicit YAMI(const Options& options);
	virtual ~YAMI();

	void start();
	void stop();

	void register_module(std::shared_ptr<const Module> inst);

	virtual
	void register_object(const std::string& name, DirectHandler_t handler);
	void register_io_error_handler(IOErrorHandler_t handler);

private:
	std::unique_ptr<yami::agent> agent_;
	std::string build_address() const;
	YAMIRequestDispatcher dispatcher_;
	std::string resolved_address_;

	yami::activity_statistics_monitor stats_callback_;
};


// value publisher callback
class overflow_functor
{
public:
	overflow_functor() = default;
	virtual yami::value_publisher_overflow_action operator() (
		const std::string& /* server_name */,
		const std::string& /* object_name */,
		const yami::serializable& /* value */)
	{
		return yami::abandon_message;
	}
};


class im_functor
{
public:
	im_functor() = default;

	virtual void operator() (yami::incoming_message& im)
	{
		im.reply();
	}
};


} // transport
} // nix

#endif
