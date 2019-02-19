#pragma once
#include "EventLoop.h"
#include "base/MutexLock.h"
#include "base/Thread.h"
#include "base/Condition.h"

using namespace std;

class EventLoopThread{
public:
	EventLoopThread();
	~EventLoopThread();

	EventLoop* startLoop();
	void threadfunc();
private:
	EventLoop *loop_;
	Thread thread_;
	mutable MutexLock mutex_;
	Condition cond_;

};
