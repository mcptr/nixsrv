#ifndef NIX_MODULE_BUILTIN_DEBUG_WORKER_HXX
#define NIX_MODULE_BUILTIN_DEBUG_WORKER_HXX


#include <memory>
#include <yami4-cpp/yami.h>

#include "nix/queue.hxx"
#include "nix/message/incoming.hxx"


namespace nix {
namespace module {
namespace debug {


class Worker
{
public:
	Worker() = delete;
	Worker(std::shared_ptr<Queue<IncomingMessage>> q, int id);
	virtual ~Worker() = default;

	void run();

protected:
	std::shared_ptr<Queue<IncomingMessage>> q_;
	int worker_id_;
	void process(IncomingMessage& ptr);
};

} // debug
} // module
} // nix

#endif
