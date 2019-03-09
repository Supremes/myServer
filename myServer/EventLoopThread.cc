#include "EventLoopThread.h"

using namespace std;

EventLoopThread::EventLoopThread():
							loop_(NULL),
							thread_(bind(&EventLoopThread::threadfunc, this), "EventLoopThread"),
							mutex_(),
							cond_(mutex_)
{

}

EventLoopThread::~EventLoopThread()
{
	if (loop_ != NULL)
    {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
	assert(!thread_.started());
	thread_.start();
	{
		MutexLockGuard lock(mutex_);
		while(loop_ == NULL)
			cond_.wait();
	}

	return loop_;
}

void EventLoopThread::threadfunc()
{
	//线程在栈上定义EventLoop对象，将其地址赋给loop_成员变量
	//notify()条件变量，唤醒startLoop()
	EventLoop loop;
	{
		MutexLockGuard lock(mutex_);
		loop_ = &loop;
		cond_.notifyOne();
	}

	loop.loop();
	loop_ = NULL;
}