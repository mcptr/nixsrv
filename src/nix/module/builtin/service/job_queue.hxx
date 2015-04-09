#ifndef NIX_MODULE_BUILTIN_SERVICE_JOB_QUEUE_HXX
#define NIX_MODULE_BUILTIN_SERVICE_JOB_QUEUE_HXX

#include <memory>
#include <unordered_map>
#include <mutex>
#include "nix/queue.hxx"
#include "nix/queue/instance_config.hxx"
#include "nix/job.hxx"
#include "nix/module/builtin.hxx"
#include "nix/message/incoming.hxx"


namespace nix {
namespace module {


using nix::Queue;
using nix::Job;

class JobQueue : public BuiltinModule
{
public:
	JobQueue() = delete;
	explicit JobQueue(std::shared_ptr<ModuleAPI> api,
					  const nix::server::Options& options);
	~JobQueue();

	bool init_queue(std::shared_ptr<nix::queue::InstanceConfig> inst);
	bool init_queue(const std::string& name, size_t size);

	// route handlers
	void submit(std::unique_ptr<IncomingMessage> msg);
	void get_work(std::unique_ptr<IncomingMessage> msg);

	void set_progress(std::unique_ptr<IncomingMessage> msg);
	void set_result(std::unique_ptr<IncomingMessage> msg);
	void get_result(std::unique_ptr<IncomingMessage> msg);


	void status(std::unique_ptr<IncomingMessage> msg);

	// admin handlers
	void manage(std::unique_ptr<IncomingMessage> msg);
private:
	const int default_queue_size_ = 50;
	long long completed_jobs_ = 0;

	void clear_all();
	void switch_all(bool state);
	void remove_all();

	bool persistent_ = false;
	std::mutex mtx_;

	std::unordered_map<std::string, Queue<Job>*> queues_;
	std::unordered_map<std::string, std::unique_ptr<Job>> in_progress_;
	// this will be used when persistent queues are not in use
	std::unordered_map<std::string, std::unique_ptr<Message>> completed_;
};


} // module
} // nix


#endif
