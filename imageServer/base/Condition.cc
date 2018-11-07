#include "Condition.h"
#include <iostream>

using namespace std;

Condition::Condition(MutexLock &mutex):
				mutex_(mutex)
{
	assert(!pthread_cond_init(&cond_, NULL));
}

Condition::~Condition()
{
	assert(!pthread_cond_destroy(&cond_));
	// if(pthread_cond_destroy(&cond_))
	// 	perror("pthread_cond_destroy error???");
	// else
	// 	cout << "success!" << endl;
}

void Condition::notifyOne()
{
	assert(!pthread_cond_signal(&cond_));

}

void Condition::notifyAll()
{
	assert(!pthread_cond_broadcast(&cond_));
}

void Condition::wait()
{
	assert(!pthread_cond_wait(&cond_, mutex_.getMutex()));
}

bool Condition::waitForTime(int seconds)
{
	struct timespec abstime;
	clock_gettime(CLOCK_REALTIME, &abstime);
	abstime.tv_sec += static_cast<time_t>(seconds);
	return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.getMutex(), &abstime);
}