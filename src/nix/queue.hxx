#ifndef NIX_QUEUE_HXX
#define NIX_QUEUE_HXX

#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>

#include <iostream>

namespace nix {


template<class T>
class Queue
{
public:
	Queue() = delete;
	Queue(const Queue& other) = delete;
	Queue& operator= (Queue& other) = delete;

	Queue(size_t queue_size)
		: queue_size_(queue_size),
		  is_closed_(false)
	{
	}

	void pop(std::unique_ptr<T>&& ptr, int timeout = -1)
	{
		ptr.reset();
		std::unique_lock<std::mutex> lock(mtx_);

		while(!is_closed_ && !ptr) {
			//std::cout << "LOOP " << std::endl;
			if(!q_.empty()) {
				ptr = std::move(q_.front());
				q_.pop();
			}

			if(!ptr) {
				if(timeout == -1) {
					//std::cout << "WAIT " << std::endl;
					cv_.wait(lock);//, [this]() { return this->is_closed(); });
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
		//std::cout << "POP EXITING " << std::endl;
	}

	void push(std::unique_ptr<T>&& elem, bool& success)
	{
		success = false;

		std::unique_lock<std::mutex> lock(mtx_);
		//std::cout << "PUSH LOCKED " << std::endl;

		if(!is_closed_ && queue_size_ && q_.size() < queue_size_) {
			q_.push(std::move(elem));
			//std::cout << "PUSHED " << q_.size() << std::endl;
			success = true;
		}
		lock.unlock();
		//std::cout << "UNLOCKED -> NOTIFY" << std::endl;
		cv_.notify_one();
	}

	size_t size() const
	{
		return q_.size();
	}

	void close()
	{
		//std::unique_lock<std::mutex> lock(mtx_);
		is_closed_ = true;
		//lock.unlock();
		cv_.notify_all();
	}

	bool is_closed() const
	{
		return is_closed_;
	}

protected:
	std::queue<std::unique_ptr<T>> q_;
	size_t queue_size_ = 0;
	std::atomic<bool> is_closed_;
	std::mutex mtx_;
	std::condition_variable cv_;
};


} // nix

#endif
