#ifndef NIX_ROUTING_ROUTE_HXX
#define NIX_ROUTING_ROUTE_HXX

#include <memory>
#include <string>
#include <functional>
#include <iostream>

#include "nix/message/incoming.hxx"


namespace nix {


class Route
{
public:
	typedef enum { SYNC, ASYNC, FUTURE, PUBLISH } ProcessingType_t;


	// ANY - allow from all - unauthenticated
	// PUBLIC - allow any public api_key
	// PRIVATE - for use by servers
	// ADMIN - for operational commands
	typedef enum { ANY, PUBLIC, API_PRIVATE, ADMIN } AccessModifier_t;

	typedef std::function<void(std::unique_ptr<IncomingMessage>)> Handler_t;

	Route() = delete;

	Route(const std::string& route,
		  Handler_t handler,
		  AccessModifier_t am = API_PRIVATE,
		  ProcessingType_t processing_type = SYNC,
		  const std::string& description = "");

	virtual ~Route() = default;

	void handle(std::unique_ptr<IncomingMessage> msg) const;

	inline const std::string& get_route() const { return route_; }
	inline AccessModifier_t get_access_modifier() const { return am_; }
	inline ProcessingType_t get_processing_type() const { return processing_type_; }
	inline const std::string& get_description() const { return description_; }

private:
	const std::string route_;
	Handler_t handler_;
	AccessModifier_t am_;
	ProcessingType_t processing_type_;
	const std::string description_;
};


} // nix

#endif
