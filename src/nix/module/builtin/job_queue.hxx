#ifndef NIX_MODULE_BUILTIN_QUEUE_HXX
#define NIX_MODULE_BUILTIN_QUEUE_HXX

#include <memory>
#include "nix/queue.hxx"
#include "nix/job.hxx"
#include "nix/module.hxx"

namespace nix {
namespace module {

using nix::Queue;
using nix::Job;

class JobQueue : public Module
{
public:
	JobQueue() = delete;
	explicit JobQueue(std::shared_ptr<ModuleAPI> api, size_t queue_size);
private:
	std::unique_ptr<Queue<Job>> submitted_;
};


} // module
} // nix


#endif
