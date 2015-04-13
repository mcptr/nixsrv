#ifndef NIX_MODULE_BUILTIN_SERVICE_RESOLVER_HXX
#define NIX_MODULE_BUILTIN_SERVICE_RESOLVER_HXX

#include <memory>
#include <unordered_map>
#include <set>
#include <mutex>
#include <thread>
#include <atomic>

#include "nix/module/builtin.hxx"
#include "nix/message/incoming.hxx"

namespace nix {
namespace module {


class Resolver : public BuiltinModule
{
public:
	Resolver() = delete;
	virtual ~Resolver() = default;
	explicit Resolver(std::shared_ptr<ModuleAPI> api,
					  const nix::server::Options& options);

private:
	void bind(std::unique_ptr<IncomingMessage> msg);
	void resolve(std::unique_ptr<IncomingMessage> msg);
	void unbind(std::unique_ptr<IncomingMessage> msg);

	void bind_service(std::unique_ptr<IncomingMessage> msg);
	void resolve_service(std::unique_ptr<IncomingMessage> msg);
	void unbind_service(std::unique_ptr<IncomingMessage> msg);

	void unbind_node(const std::string& nodename);

	std::unordered_map<std::string, std::string> nodes_;
	std::unordered_map<std::string, std::set<std::string>> services_;

	void start();
	void stop();

	std::atomic<bool> monitor_stop_flag_ {false};
	std::timed_mutex monitor_mtx_;
	std::thread monitor_thread_;
	void monitor();

};


} // module
} // nix

#endif
