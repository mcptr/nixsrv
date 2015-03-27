#ifndef NIX_CORE_ROUTING_ROUTE_HXX
#define NIX_CORE_ROUTING_ROUTE_HXX

#include <string>
#include <functional>


namespace nix {

namespace api {

// fwd
class Request_t;
class Response;

}

namespace core {


class Route
{
public:
	typedef enum { ONE_WAY, TWO_WAY, PUBSUB, CACHE, BROKER, QUEUE } Method_t;

	typedef enum { AUTH, ANY, RESTRICTED,
				   API_PUBLIC, API_PRIVATE,
				   INTERNAL } AccessModifier_t;

	typedef std::function<
		void(const api::Request_t&, api::Response& response)
		> Handler_t;

	Route() = delete;

	Route(const std::string& route,
		  Method_t method,
		  AccessModifier_t am,
		  const std::string& description = "");

	virtual ~Route() = default;

	const std::string& get_route() const;
	Method_t get_method() const;
	AccessModifier_t& get_access_modifier() const;
	const std::string& get_description() const;
	

private:
	const std::string route_;
	Method_t method_;
	AccessModifier_t am_ = RESTRICTED;
	const std::string description_;
};


} // core
} // nix

#endif
