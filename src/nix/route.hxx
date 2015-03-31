#ifndef NIX_ROUTING_ROUTE_HXX
#define NIX_ROUTING_ROUTE_HXX

#include <string>
#include <functional>
#include <iostream>

#include "nix/message/incoming.hxx"

namespace nix {


class Route
{
public:
	typedef enum { SYNC, ASYNC, DEFERRED, PUBLISH } ProcessingType_t;

	typedef enum { AUTH, ANY, RESTRICTED,
				   API_PUBLIC, API_PRIVATE,
				   INTERNAL } AccessModifier_t;

	typedef std::function<void(IncomingMessage&)> Handler_t;

	Route() = delete;

	Route(const std::string& route,
		  Handler_t& handler,
		  AccessModifier_t am = RESTRICTED,
		  ProcessingType_t processing_type = SYNC,
		  const std::string& description = "");

	virtual ~Route() = default;

	void handle(IncomingMessage& msg) const;

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
