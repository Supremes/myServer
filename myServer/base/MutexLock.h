#pragma once
#include <pthread.h>
#include <assert.h>
#define CHECK(ret) ({__typeof__(ret) err=(ret);\
                    assert(err==0);})

class MutexLock{
public:
	MutexLock()
	{
		assert(!pthread_mutex_init(&mutex_, NULL));
	}
	~MutexLock()
	{
		pthread_mutex_lock(&mutex_);
		pthread_mutex_destroy(&mutex_);
	}
	void lock()
	{
		assert(!pthread_mutex_lock(&mutex_));
	}
	void unlock()
	{
		assert(!pthread_mutex_unlock(&mutex_));
	}
	pthread_mutex_t* getMutex()
	{
		return &mutex_;
	}
private:
	pthread_mutex_t mutex_;

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
