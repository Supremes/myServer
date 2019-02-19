#pragma once
#include <pthread.h>
#include <assert.h>
#define CHECK(ret) ({__typeof__(ret) err=(ret);\
                    assert(err==0);})

class MutexLock{
public:
	MutexLock():
	is_locked(false)
	{
		assert(!pthread_mutex_init(&mutex_, NULL));
	}
	~MutexLock()
	{
		pthread_mutex_lock(&mutex_);
		//assert(!is_locked);
		pthread_mutex_destroy(&mutex_);
	}
	void lock()
	{
		is_locked = true;
		assert(!pthread_mutex_lock(&mutex_));
	}
	void unlock()
	{
		is_locked = false;
		assert(!pthread_mutex_unlock(&mutex_));
	}
	pthread_mutex_t* getMutex()
	{
		return &mutex_;
	}
	//void trylock()
private:
	pthread_mutex_t mutex_;
	bool is_locked;

};

//RAII
class MutexLockGuard{
public:
	explicit MutexLockGuard(MutexLock& mutex_):
	mutex_lock(mutex_)
	{
		mutex_lock.lock();
	}
	~MutexLockGuard()
	{
		mutex_lock.unlock();
	}

private:
	MutexLock &mutex_lock;
};
