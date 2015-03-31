#ifndef NIX_QUEUE_HXX
#define NIX_QUEUE_HXX

#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <memory>

namespace nix {


template<class T>
class Queue
{
public:
	Queue() = default;
	Queue(size_t queue_size)
		: queue_size_(queue_size)
	{
	}

	void get(std::shared_ptr<T>& ptr, int timeout = -1)
	{
		ptr.reset();
		std::unique_lock<std::mutex> lock(mtx_);

		while(!ptr) {
			if(!q_.empty()) {
				ptr = q_.back();
				q_.pop();
			}

			if(!ptr) {
				if(timeout == -1) {
					cv_.wait(lock);
				}
				else {
					std::cv_status status = 
						cv_.wait_for(lock, std::chrono::milliseconds(timeout));
					if(status == std::cv_status::timeout) {
						break;
					}
				}
			}
			else {
				break;
			}
		}

		lock.unlock();
	}

	void put(std::unique_ptr<T> elem, bool& success)
	{
		success = false;
		mtx_.lock();
		if(queue_size_ && queue_size_ < q_.size()) {
			q_.push(elem);
			success = true;
		}
		mtx_.unlock();
		cv_.notify_one();
	}

	size_t size() const
	{
		return q_.size();
	}

protected:
	std::queue<std::shared_ptr<T>> q_;
	size_t queue_size_ = 0;
	std::mutex mtx_;
	std::condition_variable cv_;
};


} // nix

#endif
