#ifndef NIX_CORE_OBJECT_POOL_HXX
#define NIX_CORE_OBJECT_POOL_HXX

#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <iostream>

const int WAIT_FOREVER = -1;

template <class T>
class ObjectPool
{
public:
	typedef std::function<T*(void)> Initializer_t;

	ObjectPool(size_t size = 1)
	{
		for(size_t i = 0 ; i < size; i++) {
			pool_.push_back(std::shared_ptr<T>(new T()));
		}
	}

	ObjectPool(Initializer_t initializer, size_t size = 1)
	{
		for(size_t i = 0 ; i < size; i++) {
			pool_.push_back(std::shared_ptr<T>(initializer()));
		}
	}

	~ObjectPool()
	{
		typename Pool_t::iterator it = pool_.begin();
		for( ; it != pool_.end(); it++) {
			it->reset();
		}
	}

	void apply(std::function<void(T&)> func)
	{
		typename Pool_t::iterator it = pool_.begin();
		for( ; it != pool_.end(); it++) {
			func(*(it->get()));
		}
	}

	void insert(T* obj)
	{
		pool_.push_back(std::shared_ptr<T>(obj));
	}

	std::shared_ptr<T> acquire(int timeout = WAIT_FOREVER)
	{
		using namespace std::chrono;
		using std::placeholders::_1;

		std::shared_ptr<T> result(nullptr);
		std::unique_lock<std::mutex> lock(mtx_);
		typename Pool_t::iterator it;
		while(!result) {
			for(it = pool_.begin() ; it != pool_.end(); it++) {
				if(it->unique()) {
					result = *it;
					break;
				}
			}

			if(!result) {
				if(timeout == WAIT_FOREVER) {
					cv_.wait(lock);
				}
				else {
					std::cv_status status = 
						cv_.wait_for(lock, milliseconds(timeout));
					if(status == std::cv_status::timeout) {
						// will result in nullptr
						break;
					}
				}
			}
		}
		lock.unlock();
		return result;
	}

	void release(std::shared_ptr<T> ptr)
	{
		ptr.reset();
		cv_.notify_all();
	}
	
	typedef std::vector<std::shared_ptr<T>> Pool_t;


protected:
	Pool_t pool_;
	std::mutex mtx_;
	std::condition_variable cv_;
};

#endif
