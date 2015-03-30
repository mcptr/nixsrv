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
class IncomingMessage;
class OutgoingMessage;

template<class T>
class Transport
{
public:
	explicit Transport(const transport::Options& options)
		: options_(options)
	{
	}

	virtual ~Transport() = default;

protected:
	const transport::Options& options_;
};



// client
template<class Parameters_t>
class ClientTransport : public Transport<Parameters_t>
{
public:
	ClientTransport() = delete;

	explicit ClientTransport(const std::string& address,
							 const transport::Options& options)
		: Transport<Parameters_t>(options),
		  address_(address)
	{
	}

	virtual ~ClientTransport() = default;

	virtual
	std::shared_ptr<IncomingMessage>
	call(const std::string& module,
		 const std::string& route,
		 const Parameters_t& parameters,
		 int timeout = 0) = 0;

	virtual
	std::shared_ptr<IncomingMessage>
	call(const std::string& module,
		 const std::string& route,
		 OutgoingMessage& msg,
		 int timeout = 0) = 0;

	virtual void send_one_way(const std::string& module,
							  const std::string& route,
							  const Parameters_t& parameters) = 0;

	virtual void send_one_way(const std::string& module,
							  const std::string& route,
							  OutgoingMessage& msg) = 0;

protected:
	const std::string address_;
};



// server
template<class Message_t>
class ServerTransport : public Transport<Message_t>
{
public:
	typedef std::vector<std::shared_ptr<const Route>> Routes_t;
	typedef std::function<void(Message_t&)> DirectHandler_t;

	ServerTransport() = delete;

	explicit ServerTransport(const transport::Options& options)
		: Transport<Message_t>(options)
	{
	}

	virtual ~ServerTransport() = default;

	virtual void start() = 0;

	virtual void stop() = 0;

	virtual void register_module(std::shared_ptr<const Module> mod) = 0;

	virtual void register_object(const std::string& name, 
								 std::function<void(Message_t&)> msg) = 0;

protected:
	Routes_t routing_;
};


} // nix

#endif
