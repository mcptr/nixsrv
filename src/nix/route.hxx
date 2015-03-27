#ifndef NIX_CORE_ROUTING_ROUTE_HXX
#define NIX_CORE_ROUTING_ROUTE_HXX

#include <string>
#include <functional>


namespace nix {


// fwd
class Request_t;
class Response;


class Route
{
public:
	typedef enum { ONE_WAY, TWO_WAY, PUBSUB, CACHE, BROKER, QUEUE } Method_t;

	typedef enum { AUTH, ANY, RESTRICTED,
				   API_PUBLIC, API_PRIVATE,
				   INTERNAL } AccessModifier_t;

	typedef std::function<
		void(const Request_t&, Response&)
		> Handler_t;

	Route() = delete;

	Route(const std::string& route,
		  Method_t method,
		  Handler_t& handler,
		  AccessModifier_t am = RESTRICTED,
		  const std::string& description = "");

	virtual ~Route() = default;

	void operator()(const Request_t& req,
					Response& res);

	const std::string& get_route() const;
	Method_t get_method() const;
	AccessModifier_t& get_access_modifier() const;
	const std::string& get_description() const;

private:
	const std::string route_;
	Method_t method_;
	Handler_t handler;
	AccessModifier_t am_;
	const std::string description_;
};


} // nix

#endif
