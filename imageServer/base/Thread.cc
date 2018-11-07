#include "Thread.h"
#include <iostream>
#include "CurrentThread.h"
using namespace std;

ThreadData::ThreadData(const threadFunc& func):
				func_(func)
{

}

void ThreadData::run()
{
	func_();
}
Thread::Thread(const threadFunc& func):
		func_(func),
		thread_id(0),
		running(false),
		joined(false),
		data(nullptr)
{

}

Thread::~Thread()
{
	if(running && !joined)
		pthread_detach(thread_id);
}
//线程运行函数
void *execute_func(void* ptr)
{
	ThreadData *mData = static_cast<ThreadData*>(ptr);
	mData->run();

}

int Thread::run()
{
	assert(!running);
	running = true;
	data = new ThreadData(func_);
	if(pthread_create(&thread_id, NULL, execute_func, data) == 0){
		//cout << "done" << endl;
		return 1;
	}else{
		running = false;
		perror("pthread_create error!");
		return -1;
	}

}

bool Thread::isRunning()
{
	return running;
}

int Thread::join()
{
	assert(running);
	assert(!joined);
	joined = true;

	return pthread_join(thread_id, NULL);
}

pthread_t Thread::getId()
{
	return thread_id;
}

namespace CurrentThread
{
	__thread int t_cachedTid = 0;
	__thread char t_tidString[32];
	__thread int t_tidStringLength = 6;
	__thread const char* t_threadName = "default";
}
void CurrentThread::cacheTid()
{
	if (t_cachedTid == 0)
	{
		t_cachedTid = gettid();
		t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
	}
}
