#pragma once
#include "Epoller.h"
#include "EventLoop.h"
#include "Channel.h"
#include "base/utility.h"
#include "EventLoopThreadPool.h"
using namespace std;

class server{
public:
	server(EventLoop *loop, int port, int threadnum);
	~server(){}
	void handleNewConnection();
	void handleThisConnection()
	{
		// cout << "server::handleThisConnection..." << endl;
		// string str = "connect success!\n";
		// if(writen(listenfd_, str) <= 0)
		// 	perror("write first connect");
		loop_->updatePoller(acceptChannel_);
	}
	void start();
	//int getListenFd_(){	return listenfd_; }
private:
	EventLoop *loop_;
	EventLoopThreadPool *pool_;
	int port_;
	int listenfd_;
	spChannel acceptChannel_;
	int threadnum_;
};
