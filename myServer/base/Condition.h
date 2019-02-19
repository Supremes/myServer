#pragma once
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include "MutexLock.h"

class Condition{
public:
	Condition(MutexLock &mutex);
	~Condition();
	void notifyOne();
	void notifyAll();
	void wait();
	bool waitForTime(int seconds);
private:
	pthread_cond_t cond_;
	MutexLock &mutex_;

};
