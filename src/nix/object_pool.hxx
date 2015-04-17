#ifndef NIX_OBJECT_POOL_HXX
#define NIX_OBJECT_POOL_HXX

#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <iostream>


namespace nix {


const int WAIT_FOREVER = -1;

template <class T>
class ObjectPool
{
public:
	typedef std::function<T*(void)> Initializer_t;
	ObjectPool() = default;

	ObjectPool(size_t size)
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

	virtual ~ObjectPool()
	{
		typename Pool_t::iterator it = pool_.begin();
		for( ; it != pool_.end(); it++) {
			it->reset();
		}
	}

	void clear()
	{
		pool_.clear();
	}

	size_t size() const
	{
		return pool_.size();
	}

	void apply(std::function<void(T&)> func, int pos = -1)
	{
		mtx_.lock();
		if(pos > -1) {
			func(*(pool_[pos].get()));
		}
		else {
			typename Pool_t::iterator it = pool_.begin();
			for( ; it != pool_.end(); it++) {
				func(*(it->get()));
			}
		}
		mtx_.unlock();
	}

	void insert(T* obj)
	{
		pool_.push_back(std::shared_ptr<T>(obj));
	}

	bool acquire(std::shared_ptr<T>& ptr, int timeout = WAIT_FOREVER)
	{
		ptr.reset();
		typename Pool_t::iterator it;
		std::unique_lock<std::mutex> lock(mtx_);

		while(!ptr) {
			for(it = pool_.begin() ; it != pool_.end(); it++) {
				if(it->unique()) {
					ptr = std::shared_ptr<T>(
						it->get(),
						std::bind(&ObjectPool<T>::notify, this)
					);
					break;
				}
			}

			if(!ptr) {
				if(timeout == WAIT_FOREVER) {
					cv_.wait(lock);
				}
				else {
					std::cv_status status = 
						cv_.wait_for(lock, std::chrono::milliseconds(timeout));
					if(status == std::cv_status::timeout) {
						lock.unlock();
						return false;
					}
				}
			}
		}

		lock.unlock();
		return !!ptr;
	}

	typedef std::vector<std::shared_ptr<T>> Pool_t;
protected:
	Pool_t pool_;
	std::mutex mtx_;
	std::condition_variable cv_;

	void notify()
	{
		cv_.notify_all();
	}
};


} // nix

#endif
