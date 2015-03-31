#ifndef NIX_MODULE_BUILTIN_QUEUE_HXX
#define NIX_MODULE_BUILTIN_QUEUE_HXX

#include <memory>
#include <map>
#include "nix/queue.hxx"
#include "nix/queue/instance_config.hxx"
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
	~JobQueue();

	explicit JobQueue(std::shared_ptr<ModuleAPI> api, size_t queue_size);
	void init_queue(std::shared_ptr<nix::queue::InstanceConfig> inst);
private:
	std::map<const std::string, Queue<Job>*> queues_;
};


} // module
} // nix


#endif
