#pragma once
#include "base/utility.h"
#include "EventLoop.h"
#include "Channel.h"
#include "timer.h"
#include <memory>

using namespace std;

enum HttpMethod
{
	HTTP_GET = 1,
	HTTP_POST,
	HTTP_HEAD
};

enum ConnectionState{
	HTTP_CONNECTED = 1,
	HTTP_DISCONNECTING,
	HTTP_DISCONNECTED
};

enum ParseState{
	PARSE_LINE = 1,
	PARSE_HEAD,
	PARSE_BODY
};


struct HttpRequestLine{
	string method;
	string url;
	string version;
};

struct HttpRequestHead{
	string Connection;
	string Host;
    string Agent;
    string contentType;
	string Date;
};

struct HttpRequestBody{
	string text;
};



class httpData: public enable_shared_from_this<httpData>{
public:
	httpData(EventLoop *loop, int connfd);
	~httpData(){ close(connfd_); cout << "close" << endl;}
	
	void seperateTimer();

	void handleClose();
	void handleError();
	//解析请求行
	bool parseLine(string &data);
	//解析请求头
	bool parseHeader(string &data);

	//根据httpData中的请求数据进行响应
	bool doHttpData();
	void handleNewConn();

	void linkTimer(shared_ptr<timerNode> mtimer)
    {
        // shared_ptr重载了bool, 但weak_ptr没有
        timer_ = mtimer; 
    }

    shared_ptr<Channel> const getChannel()
    {
        return spChannel;
    }
	EventLoop* getLoop(){ return loop_; }
private:

	HttpRequestLine line_;
	HttpRequestHead head_;
	HttpRequestBody body_;

	int connfd_;
	bool error_;
	string inBuffer_;
	string outBuffer_;

	ConnectionState connectionState_;
	ParseState state_;

	EventLoop* loop_;
	shared_ptr<Channel> spChannel;
	//httpData与Timer之间存在循环引用,可能导致内存泄漏
	//使用weak_ptr可解决该问题(仅仅在程序员可预见该错误(循环引用)的情况下使用)
	weak_ptr<timerNode> timer_;
	
	void handleConn();
	void handleRead();
	void handleWrite();
	void doError(string error = "default");
};

typedef shared_ptr<httpData> spHttpData;