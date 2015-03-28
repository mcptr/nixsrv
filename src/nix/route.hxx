#ifndef NIX_ROUTING_ROUTE_HXX
#define NIX_ROUTING_ROUTE_HXX

#include <string>
#include <functional>

#include "nix/impl_types.hxx"

namespace nix {


// fwd
class Response;


class Route
{
public:
	typedef enum { ONE_WAY, TWO_WAY, PUBSUB, CACHE, BROKER, QUEUE } Method_t;

	typedef enum { AUTH, ANY, RESTRICTED,
				   API_PUBLIC, API_PRIVATE,
				   INTERNAL } AccessModifier_t;

	typedef std::function<
		void(const impl::Request_t&, Response&)
		> Handler_t;

	Route() = delete;

	Route(const std::string& route,
		  Method_t method,
		  Handler_t& handler,
		  AccessModifier_t am = RESTRICTED,
		  const std::string& description = "");

	virtual ~Route() = default;

	void handle(const impl::Request_t& req, Response& res) const;

	inline const std::string& get_route() const { return route_; }
	inline Method_t get_method() const { return method_; }
	inline AccessModifier_t get_access_modifier() const { return am_; }
	inline const std::string& get_description() const { return description_; }

private:
	const std::string route_;
	Method_t method_;
	Handler_t handler_;
	AccessModifier_t am_;
	const std::string description_;
};


} // nix

#endif
