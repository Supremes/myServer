#include "Epoller.h"
#include "base/utility.h"
#include <assert.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
using namespace std;

const int EPOLLWAIT_TIME = 10000;
const int EVENT_SUM = 2048;

Epoller::Epoller():
		epollfd_(epoll_create1(0)),
		eventList(EVENT_SUM),
		timerNodeManager_(new timerNodeManager())
{
	assert(epollfd_ > 0);
}

Epoller::~Epoller()
{

}

vector<spChannel> Epoller::poll()
{
	while(true){
		vector<spChannel> req_Channel;
		int event_count = epoll_wait(epollfd_, &*eventList.begin(), static_cast<int>(eventList.size()), EPOLLWAIT_TIME);
		if(event_count < 0)
			perror("epoll_wait() error");
		if(event_count > 0){
			for(int i = 0; i < event_count; i++){
				int fd = eventList[i].data.fd;
				spChannel ch_cur = channelList[fd];
				if(ch_cur){
					ch_cur->setRevents(eventList[i].events);
					ch_cur->setEvents(0);
					req_Channel.push_back(ch_cur);
				}
				else{
					//实现LOG日志
					//cout << "spChannel is invalid" << endl;
				}
			}
			if(req_Channel.size() > 0)
				return req_Channel;
		}
	}
}
void Epoller::epoll_add(spChannel channel, int timeout)
{
	int fd = channel->getFd();
	if (timeout > 0)
	{
		timerNodeManager_->addTimer(channel->getHolder(), timeout);
		dataList[fd] = channel->getHolder();
	}
	struct epoll_event events_;
	//bzero(&events_, sizeof events_);
	//events_.data.ptr = channel->
	events_.data.fd = fd;
	events_.events = channel->getEvents();

	channelList[fd] = channel;
	if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &events_) < 0)
	{
		perror("epoll_ctl error");
		channelList[fd].reset();
	}
}

void Epoller::epoll_mod(spChannel channel, int timeout)
{
	int fd = channel->getFd();
	assert(channelList.find(fd) != channelList.end());
    assert(channelList[fd] == channel);

	if(timeout > 0)
	{
		timerNodeManager_->addTimer(channel->getHolder(), timeout);
	}
	struct epoll_event events_;
	bzero(&events_, sizeof events_);
	events_.data.fd = fd;
	events_.events = channel->getEvents();

	if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &events_) < 0)
	{
		perror("epoll_ctl error");
		channelList[fd].reset();
	}
}

void Epoller::epoll_del(spChannel channel)
{
	int fd = channel->getFd();
	assert(channelList.find(fd) != channelList.end());
    assert(channelList[fd] == channel);

	struct epoll_event events_;
	bzero(&events_, sizeof events_);
	events_.data.fd = fd;
	events_.events = channel->getEvents();
	
	if(epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &events_) < 0)
	{
		perror("epoll del failed!");
	}
	channelList[fd].reset();
	dataList[fd].reset();
	cout << "fd " << fd << "is deleted"  << endl;
}

void Epoller::handleExpired()
{
	timerNodeManager_->handleExpiredEvent();
}
