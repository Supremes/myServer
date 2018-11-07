#include "Channel.h"
// #include "EventLoop.h"
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
		readCallback_();
	}
}

void Channel::handleWrite()
{
	if(writeCallback_)
	{
		writeCallback_();
	}
}

void Channel::handleConn()
{
	if(connCallback_)
	{
		connCallback_();
	}
}

void Channel::handleError()
{
	if(errorCallback_)
	{
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

}