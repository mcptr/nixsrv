#ifndef NIX_MODULE_BUILTIN_DEBUG_HXX
#define NIX_MODULE_BUILTIN_DEBUG_HXX

#include <memory>
#include <mutex>
#include <yami4-cpp/yami.h>
#include <atomic>
#include "nix/module/builtin.hxx"
#include "nix/queue.hxx"
#include "nix/message/incoming.hxx"
#include "nix/server/options.hxx"
#include "debug/worker.hxx"

namespace nix {
namespace module {


class Debug : public BuiltinModule
{
public:
	Debug() = delete;
	virtual ~Debug() = default;
	explicit Debug(std::shared_ptr<ModuleAPI> api,
				   const nix::server::Options& options);

	void debug_sync(std::unique_ptr<IncomingMessage> msg);
	void debug_async(std::unique_ptr<IncomingMessage> msg);
	void start();
	void stop();
private:
	size_t replies_ = 0;
	std::shared_ptr<Queue<IncomingMessage>> q_;
	std::vector<debug::Worker> workers_;
	std::vector<std::thread> threads_;
};


} // module
} // nix

#endif
