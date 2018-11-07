#pragma once
#include "Channel.h"
#include <memory>
#include <vector>
#include <map>

using namespace std;

class timerNodeManager;

class Epoller{
public:
	Epoller();
	~Epoller();	

	void epoll_mod(spChannel channel, int timeout);
	void epoll_del(spChannel channel);
	void epoll_add(spChannel channel, int timeout);
	//poll获得当前活动的IO事件被绑定到Channel
	vector<spChannel> poll();
	void handleExpired();

private:
	int epollfd_;
	vector<struct epoll_event> eventList;
	//用于在poll循环当中进行channel验证（epoll成员函数添加的和epoll_wait所监听到的）
	map<int, spChannel > channelList;
	//保存了所有一次epoll_wait()返回的活动httpData(fd封装在httpData类中)列表
	//map<int, spChannel > dataList;
	timerNodeManager *timerNodeManager_;

};
