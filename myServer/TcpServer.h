#pragma once
#include "Epoller.h"
#include "EventLoop.h"
#include "Channel.h"
#include "base/utility.h"
#include "base/MutexLock.h"
#include "EventLoopThreadPool.h"
#include "httpData.h"
using namespace std;

class server{
public:
	server(EventLoop *loop, int port, int threadnum);
	~server();
	void handleNewConnection();
	void handleThisConnection()
	{
		loop_->updatePoller(acceptChannel_);
	}
	void start();
	void removeConnectionInLoop(const spHttpData myConn);

private:
	typedef map<int, spHttpData> ConnectionMap;
	EventLoop *loop_;
	EventLoopThreadPool *pool_;
	int port_;
	int listenfd_;
	spChannel acceptChannel_;
	int threadnum_;
	MutexLock mutex_;
	ConnectionMap connections_;
};
