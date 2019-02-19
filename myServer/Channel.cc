#include "Channel.h"
#include <iostream>
using namespace std;

#include "EventLoop.h"
//#include "httpData.h"

Channel::Channel(EventLoop *loop, int fd):
			loop_(loop),
			fd_(fd),
			events_(0)
{

}
void Channel::handleRead()
{
	if(readCallback_)
	{
		//cout << "handleRead... fd = " << fd_ << endl;
		readCallback_();
	}
}

void Channel::handleWrite()
{
	if(writeCallback_)
	{
		cout << "handleWrite..." << endl;
		writeCallback_();
	}
}

void Channel::handleConn()
{
	if(connCallback_)
	{
		cout << "handleConn..." << endl;
		connCallback_();
	}
}

void Channel::handleError()
{
	if(errorCallback_)
	{
		cout << "handleError..." << endl;
		errorCallback_();
	}
}

void Channel::handleEvents()
{
	if(revents_ & EPOLLERR){
		handleError();
	}
	if(revents_ & EPOLLIN){
		handleRead();
	}
	if(revents_ & EPOLLOUT){
		handleWrite();
	}
	handleConn();
}

void Channel::remove()
{
	loop_->removeFromPoller(shared_from_this());
}

void Channel::disableAll()
{
	events_ = 0;
	loop_->updatePoller(shared_from_this());
}