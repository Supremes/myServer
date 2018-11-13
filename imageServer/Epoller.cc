#include "Epoller.h"
#include "timer.h"
#include <assert.h>
#include <iostream>
#include <unistd.h>
#include "timer.h"
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

// vector<spChannel> Epoller::poll()
// {
// 	while(true){
// 		vector<spChannel> req_Channel;
// 		int event_count = epoll_wait(epollfd_, &*eventList.begin(), static_cast<int>(eventList.size()), EPOLLWAIT_TIME);
// 		if(event_count < 0)
// 			perror("epoll_wait() error");
// 		if(event_count > 0){
// 			//cout << "epoll_wait's event_count = " << event_count << endl;
// 			for(int i = 0; i < event_count; i++){
// 				int fd = eventList[i].data.fd;
// 				spChannel ch_cur = channelList[fd];

// 			//	cout << "epoll fd = " << fd << endl;
				
// 				if(ch_cur){
// 					ch_cur->setRevents(eventList[i].events);
// 					ch_cur->setEvents(0);
// 					req_Channel.push_back(ch_cur);
// 				}
// 				else{
// 					//实现LOG日志
// 					//cout << "spChannel is invalid" << endl;
// 				}
// 			}
// 			if(req_Channel.size() > 0)
// 				return req_Channel;
// 		}
// 	}
// }
// 返回活跃事件数
vector<spChannel>  Epoller::poll()
{
    while (true)
    {
        int event_count = epoll_wait(epollfd_, &*eventList.begin(), eventList.size(), EPOLLWAIT_TIME);
        if (event_count < 0)
            perror("epoll wait error");
        vector<spChannel>  req_data = getEventsRequest(event_count);
        if (req_data.size() > 0)
            return req_data;
    }
}

// 分发处理函数
vector<spChannel>  Epoller::getEventsRequest(int events_num)
{
    vector<spChannel>  req_data;
    for(int i = 0; i < events_num; ++i)
    {
        // 获取有事件产生的描述符
        int fd = eventList[i].data.fd;

        spChannel cur_req = channelList[fd];
            
        if (cur_req)
        {
            cur_req->setRevents(eventList[i].events);
            cur_req->setEvents(0);
            // 加入线程池之前将Timer和request分离
            //cur_req->seperateTimer();
            req_data.push_back(cur_req);
        }
        else
        {
            //LOG << "SP cur_req is invalid";
        }
    }
    return req_data;
}
void Epoller::epoll_add(spChannel channel, int timeout)
{
	int fd = channel->getFd();
	cout << "epoll_add fd: " << fd << endl;
	if (timeout > 0)
	{
		timerNodeManager_->addTimer(channel->getHolder(), timeout);
		//dataList.insert(make_pair(fd, channel->getHolder()));
	}
	struct epoll_event events_;
	events_.data.fd = fd;
	events_.events = channel->getEvents();
	//为什么需要该操作
	//channel->EqualAndUpdateLastEvents();
	channelList[fd] = channel;

	if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &events_) < 0)
	{
		perror("epoll_ctl error");
		channelList.find(fd)->second.reset();
	}
}

void Epoller::epoll_mod(spChannel channel, int timeout)
{
	int fd = channel->getFd();
	if(timeout > 0)
	{
		timerNodeManager_->addTimer(channel->getHolder(), timeout);
	}
	struct epoll_event events_;
	events_.data.fd = fd;
	events_.events = channel->getEvents();

	//channelList.find(fd)->second = channel;
	if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &events_) < 0)
	{
		perror("epoll_ctl error");
		channelList.find(fd)->second.reset();
	}
}

void Epoller::epoll_del(spChannel channel)
{
	int fd = channel->getFd();
	struct epoll_event events_;
	events_.data.fd = fd;
	events_.events = channel->getLastEvents();

	if(epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &events_) < 0)
	{
		perror("epoll del failed!");
	}
	//dataList.find(fd)->second.reset();
	channelList.find(fd)->second.reset();
	close(fd);

}

void Epoller::handleExpired()
{
	timerNodeManager_->handleExpiredEvent();
}
