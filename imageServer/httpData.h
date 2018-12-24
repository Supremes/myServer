#pragma once
#include "base/utility.h"
#include "EventLoop.h"
#include "Channel.h"

#include <memory>

using namespace std;

enum HttpVersion
{
	HTTP_1_0 = 1,
	HTTP_1_1
};

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

class MimeType{
private:
	static void init();
	static unordered_map<string, string> mime;
	MimeType();
	MimeType(const MimeType &m);
public:
	static string getMime(const string &suffix);
private:
	//设置线程变量的原因
	static pthread_once_t& once_control;
};

struct HttpRequestLine{
	string method;
	string url;
	string version;
};

struct HttpRequestHead{
	string Connection;
	string Host;
    string Accept;
    string contentType;
};

struct HttpRequestBody{
	string text;
};



class httpData: public enable_shared_from_this<httpData>{
public:
	httpData(EventLoop *loop, int connfd);
	//~httpData();
	
	void seperateTimer();

	void handleClose();
	void handleError();
	//解析请求行
	bool parseLine();
	//解析请求头
	bool parseHeader();
	//解析请求报文体
	void parseRequest();
	//根据httpData中的请求数据进行响应
	bool doHttpData();

	void newEvent();

	void linkTimer(shared_ptr<timerNode> mtimer)
    {
        // shared_ptr重载了bool, 但weak_ptr没有
        timer_ = mtimer; 
    }

    shared_ptr<Channel> const getChannel()
    {
        return spChannel;
    }
private:

	HttpRequestLine line_;
	HttpRequestHead head_;
	HttpRequestBody body_;

	int connfd_;
	bool error_;
	string inBuffer_;
	string outBuffer_;

	ConnectionState connectionState_;
	//长连接的判断？
	bool keepAlive_;
	//初始化Channel需要用到EventLoop
	EventLoop* loop_;
	shared_ptr<Channel> spChannel;
	//疑问：为什么选择weak_ptr而不是shared_ptr
	//httpData与Timer之间存在循环引用,可能导致内存泄漏
	//使用weak_ptr可解决该问题(仅仅在程序员可预见该错误(循环引用)的情况下使用)
	weak_ptr<timerNode> timer_;
	ParseState state_;
	//细节 看一下,设置成员属性为private的原因
	void handleConn();
	void handleRead();
	void handleWrite();
};