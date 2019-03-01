#pragma once
#include "Epoller.h"
#include "Channel.h"
#include "base/MutexLock.h"
#include "base/CurrentThread.h"
#include "base/Thread.h"
#include <vector>
#include <memory>
#include <assert.h>

using namespace std;


class EventLoop{
public:
	typedef function<void()> Functor;
	
	EventLoop();
	~EventLoop();
	bool isLoopInThread(){	return threadId_ == CurrentThread::tid();	}
	void assertInLoopThread()
	{
		assert(isLoopInThread());
	}
	void loop();
	void runInLoop(const Functor&);
	void queueInLoop(const Functor&);

	void addToPoller(shared_ptr<Channel> channel, int timeout = 0)
	{
		epoller_->epoll_add(channel, timeout);
	}
	void updatePoller(shared_ptr<Channel> channel, int timeout = 0)
	{
		epoller_->epoll_mod(channel, timeout);
	}
	void removeFromPoller(shared_ptr<Channel> channel)
	{
		epoller_->epoll_del(channel);
	}
	void handleRead();
	void handleConn();
	
	void doPendingFunctors();
	void quit();
	void wakeup();
private:
	bool looping_;
	int threadId_;
	//用于唤醒IO线程
	int wakeupfd_;
	shared_ptr<Epoller> epoller_;
	//用于处理wakeupfd_上的readable事件
	shared_ptr<Channel> wakeupChannel_;
	//将未在IO线程所调用的函数加入到队列当中（统一在IO线程执行任务回调可以避免线程安全性的问题）
	vector<Functor> pendingFunctors_;
	//调用队列中分发事件的标志
	bool callingPendingFunctors_;
	mutable MutexLock mutex_;
	//控制loop()循环的开始与结束
	bool quit_;
};
