#include "httpData.h"
#include "timer.h"
#include "Channel.h"
#include "EventLoop.h"


using namespace std;
const int DEFAULT_EXPIRED_TIME = 2000;//ms
//初始化列表的成员初始化顺序是按照变量的声明顺序的
httpData::httpData(EventLoop* loop, int connfd):
			connfd_(connfd),
			error_(false),
			connectionState_(HTTP_CONNECTED),
			state_(PARSE_LINE),
			loop_(loop),
			spChannel(new Channel(loop_, connfd_))

{

	spChannel->setReadResponse(bind(&httpData::handleRead, this));
	spChannel->setWriteResponse(bind(&httpData::handleWrite, this));
	spChannel->setConnResponse(bind(&httpData::handleConn, this));
}


void httpData::seperateTimer()
{
	if(timer_.lock()){
		shared_ptr<timerNode> this_timer(timer_.lock());
		this_timer->clearRequests();
		timer_.reset();
		cout << "ok" << endl;
	}

}
void httpData::handleConn()
{
	seperateTimer();
	//if(error_)
		loop_->queueInLoop(bind(&httpData::handleClose, shared_from_this()));
}
void httpData::handleClose()     
{
	connectionState_ = HTTP_DISCONNECTED;
	shared_ptr<httpData> guard(shared_from_this());
	loop_->removeFromPoller(spChannel);
}
void httpData::handleNewConn()
{
	spChannel->setEvents(EPOLLIN | EPOLLOUT);
	loop_->addToPoller(spChannel, DEFAULT_EXPIRED_TIME);
	cout << "add success" << endl;
}
void httpData::handleRead()
{
	do{
		bool zero = false;
		int numOfRead = readn(connfd_, inBuffer_, zero);
		if(numOfRead < 0){
			error_ = true;
			//doError();
		}else if(zero){
			//有请求但是读不到数据，可能是来自网络的数据未到达或者对方端口已关闭或Request Aborted
			//统一按照对端已经关闭处理
			connectionState_ = HTTP_DISCONNECTING;
			error_ = true;
			if(numOfRead == 0)
				break; 
		}
		int index = inBuffer_.find_first_of("\r", 0);
		vector<string> parseString;
		if(index != string::npos)
			parseString.push_back(inBuffer_.substr(0, index));
		if(index + 2 < inBuffer_.size())
			parseString.push_back(inBuffer_.substr(index + 2, inBuffer_.size() - index - 2));
		//解析http字段的条件判断
		if(state_ == PARSE_LINE){
			if(parseLine(parseString[0]))
				state_ = PARSE_HEAD;
			else
				error_ =  true;
		}
		if(state_ == PARSE_HEAD){
			if(parseHeader(parseString[1]))
				state_ = PARSE_BODY;
			else
				error_ = true;
		}
		if(state_ == PARSE_BODY){
			//根据httpData中的请求数据进行响应
			if(doHttpData())
				state_ = PARSE_FIN;
			else
				error_ = true;
		}
	}while(false);
	
	if(!error_ && state_ == PARSE_FIN){
		state_ = PARSE_LINE;
		inBuffer_.clear();
		// if (timer_.lock())
    	// {
		// 	shared_ptr<timerNode> my_timer(timer_.lock());
		// 	my_timer->clearRequests();
		// 	my_timer.reset();
    	// }
	}

}
void httpData::handleWrite()
{
	if(connectionState_ != HTTP_DISCONNECTED && !error_ ){
		uint32_t &events_temp = spChannel->getEvents();
		if(writen(connfd_, outBuffer_) < 0){
			perror("writen error");
			events_temp = 0;
			error_ = true; 
		}

	}

}


void httpData::doError(string error)
{
	if(error == "default")
		outBuffer_ += "请求错误...\n";
	else if(error == "404")
		outBuffer_ += "请求资源不存在...\n";
}
//解析请求行
bool httpData::parseLine(string &data)
{
	vector<string> res;
	splitString(data, " ", res);
	if(res.size() != 3)
		return false;
	line_.method = res[0];
	line_.url = res[1];
	line_.version = res[2];
	
	int pos = inBuffer_.find("\n");
	inBuffer_.substr(pos + 1, inBuffer_.size() - pos - 1);
	
	return true;
}
//解析请求头
bool httpData::parseHeader(string &data)
{
	vector<string> headers;
	splitString(data, "\r\n", headers);
	
	if(headers.size() == 0)
		return false;

	for(int i = 0; i < headers.size(); i++){
		if(headers[i].size() == 0)	
			continue;
		vector<string> res;
		splitString(headers[i], ": ", res);
		if(res[0] == "Connection"){
			head_.Connection = res[1];
		}else if(res[0] == "HOST"){
			head_.Host = res[1];
		}else if(res[0] == "Date"){
            head_.Date = res[1];
        }else if(res[0] == "Content-Type"){
            head_.contentType = res[1];
		}else if(res[0] == "User-Agent")
			head_.Agent = res[1];
		
	}

	int pos = inBuffer_.find("\n");
	inBuffer_.substr(pos + 1, inBuffer_.size() - pos - 1);
	
	return true;
}

//根据httpData中的请求数据进行响应
bool httpData::doHttpData()
{
    //解析请求行
	if(line_.method == "GET" || line_.method == "HEAD"){
        //cout << "handle Headers..." << endl;
        //解析请求头
		if(head_.Connection.size())
        	outBuffer_ = "Connection : " + head_.Connection + "\r\n";
		if(head_.Host.size())
        	outBuffer_ = "HOST" + head_.Host + "\r\n";
		if(head_.Agent.size())
        	outBuffer_ = "User-Agent" + head_.Agent + "\r\n";
		if(head_.contentType.size())
        	outBuffer_ = "Content-Type: " + head_.contentType + "\r\n";
		if(head_.Date.size())
			outBuffer_ = "Date: " + head_.Date + "\r\n";
        
        string fileName = line_.url;
        struct stat fileInfo;
        if(stat(fileName.c_str(), &fileInfo) < 0){
            doError();
            return false;
        }

        if(line_.method == "HEAD")
            return true;
        //解析url
        int fileFd = open(fileName.c_str(), O_RDONLY);
        if(fileFd < 0){
			outBuffer_.clear();
			//doError(fd_, 404, "Not Found!");
            doError("404");
			return false;  
        }

        return true;

	}
    else if(line_.method == "POST"){
		outBuffer_ = "This method comes with 'POST'";
		return true;
	}
	else
		return false;
}