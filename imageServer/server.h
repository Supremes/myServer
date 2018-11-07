#pragma once
#include "Epoller.h"
#include "EventLoop.h"
#include "Channel.h"

using namespace std;

class server{
public:
	server(EventLoop *loop, int port, int threadnum);
	~server(){}
	void handleNewConnection();
	void handleThisConnection()
	{
		loop_->updatePoller(acceptChannel_);
	}
	void start();

private:
	EventLoop *loop_;
	int port_;
	int listenfd_;
	spChannel acceptChannel_;
	int threadnum_;
};
