#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <sys/epoll.h>
#include <functional>
#include <iostream>
using namespace std;

class EventLoop;
class imageData;

class Channel{
public:
	Channel(EventLoop*, int);
	//~Channel();
	typedef function<void()> callBack_;
	void setReadResponse(callBack_ readCallback)
	{
		readCallback_ = readCallback;
	}
	void setWriteResponse(callBack_ writeCallback)
	{
		writeCallback_ = writeCallback;
	}
	void setConnResponse(callBack_ connCallback)
	{
		connCallback_ = connCallback;
	}
	void setErrorResponse(callBack_ errorCallback)
	{
		errorCallback_ = errorCallback;
	}
	void handleRead();
	void handleWrite();
	void handleConn();
	void handleError();

	void handleEvents();

	void setHolder(shared_ptr<imageData> holder)
	{
		holder_ = holder;
	}
	shared_ptr<imageData> getHolder()
	{
		shared_ptr<imageData> ret(holder_.lock());
		//shared_ptr<imageData> ret(holder_);
		return ret;
	}

	int getFd() const { return fd_; }

	void setRevents(uint32_t Revents)
	{
		revents_ = Revents;
	}

	void setEvents(uint32_t events)
	{
		events_ = events;
	}

	uint32_t& getEvents()
	{
		return events_;
	}

	uint32_t& getLastEvents()
	{
		return lastEvents_;
	}

	bool EqualAndUpdateLastEvents()
	{
		bool ret = (lastEvents_ == events_);
		lastEvents_ = events_;
		return ret;
	}
	void disableAll();
	void remove();
private:
	EventLoop *loop_;
	//方便找到负责的IO事件（imageData封装了IO的读写监听事件）
	weak_ptr<imageData> holder_;
	//shared_ptr<imageData> holder_;
	callBack_ readCallback_;
	callBack_ writeCallback_;
	callBack_ connCallback_;
	callBack_ errorCallback_;


	//Channel对象负责一个文件描述符的IO事件分发
	int fd_;
	
	uint32_t events_;	//由用户设置
	uint32_t revents_;	//由EventLoop/Epoller设置
	uint32_t lastEvents_;

};

typedef shared_ptr<Channel> spChannel;