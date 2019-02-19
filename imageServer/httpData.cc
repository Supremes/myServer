#include "httpData.h"
#include "timer.h"

using namespace std;

//初始化列表的成员初始化顺序是按照变量的声明顺序的
httpData::httpData(EventLoop* loop, int connfd):
			connfd_(connfd),
			error_(false),
			keepAlive_(false),
			loop_(loop),
			spChannel(new Channel(loop_, connfd_))
			// method(HTTP_GET),
			// version(HTTP_1_1),
			//state_(PARSE_LINE)

{
	// spChannel->setReadResponse(bind(&httpData::handleRead, this));
	// spChannel->setWriteResponse(bind(&httpData::handleWrite, this));
	// spChannel->setConnResponse(bind(&httpData::handleConn, this));
	spChannel->setReadResponse(bind(&httpData::handleRead, this));
	spChannel->setWriteResponse(bind(&httpData::handleWrite, this));
	//spChannel->setConnResponse(bind(&httpData::handleConn, this));

}

string MimeType::getMime(const string& suffix)
{
	//需要设置线程变量初始化的原因是什么？
	//pthread_once(&once_control, MimeType::init);
	if(mime.find(suffix) == mime.end())
		return mime["default"];
	else
		return mime[suffix];
}
void httpData::seperateTimer()
{
	shared_ptr<timerNode> this_timer(timer_.lock());
	//this_timer.clearRequests();
	this_timer.reset();
}
void httpData::handleClose()
{
	connectionState_ = HTTP_DISCONNECTED;
	//设置guard共享智能指针的原因是什么
	shared_ptr<httpData> guard(shared_from_this());
	loop_->removeFromPoller(spChannel);
}

void httpData::handleRead()
{
	//uint32_t &events = spChannel->getEvents();
	do{
		bool zero = false;
		int numOfRead = readn(connfd_, inBuffer_, zero);
		if(numOfRead < 0){
			error_ = true;
			//待编写
			doError();
		}else if(zero){
			//有请求但是读不到数据，可能是来自网络的数据未到达或者对方端口已关闭或Request Aborted
			//统一按照对端已经关闭处理
			connectionState_ = HTTP_DISCONNECTING;
			error_ = true;
			if(numOfRead == 0)
				break; 
		}
		//解析http字段的条件判断
		if(state_ == PARSE_LINE){
			if(parseLine())
				state_ = PARSE_HEAD;
		}
		if(state_ == PARSE_HEAD){
			if(parseHeader())
				state_ = PARSE_BODY;
		}
		if(state_ == PARSE_BODY){
			parseRequest();
		}
		//根据httpData中的请求数据进行响应
		doHttpData();

	}while(false);

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
		//局部变量的events再给其赋值有什么作用呢
		// if(outBuffer_ > 0)
		// 	events_temp |= EPOLLOUT;
	}

}
void httpData::handleNewConn()
{
	spChannel->setEvents(EPOLLIN | EPOLLET);
	loop_->addToPoller(spChannel, 0);
	connectionState_ = HTTP_CONNECTED;
}

void httpData::doError(string error = "default")
{
	if(error == "default")
		outBuffer_ += "请求错误...\n"
	else if(errorNumber == "404")
		outBuffer_ += "请求资源不存在...\n";
}
//解析请求行
bool httpData::parseLine()
{
	vector<string> res;
	splitString(inBuffer_, " ", res);
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
bool httpData::parseHeader()
{
	vector<string> headers;
	splitString(inBuffer_, "\r\n", headers);
	
	if(headers.size() != 2)
		return false;

	for(int i = 0; i < headers.size(); i++){
		vector<string> res;
		splitString(headers[i], ": ", res);
		if(res[0] == "Connection"){
			head_.Connection = res[1];
		}else if(res[0] == "HOST"){
			head_.Host = res[1];
		}else if(res[0] == "Accept"){
            head_.Accept = res[1];
        }else if(res[0] == "Content-Type")
            head_.contentType = res[1];
	}

	int pos = inBuffer_.find("\n");
	inBuffer_.substr(pos + 1, inBuffer_.size() - pos - 1);

	return true;
}
//解析请求报文体
void httpData::parseRequest()
{
	if(inBuffer_.size())
		body_.text += inBuffer_;
}
//根据httpData中的请求数据进行响应
bool httpData::doHttpData()
{
    //解析请求行
	if(line_.method == "GET" || line_.method == "HEAD"){
		// outBuffer_ += "This method comes with 'GET' and 'HEAD'";

        // string fileName = line_.url;
        // if(open(fileName, O_WRONLY) == -1){
        //     outBuffer_ += "URL failed\n";
        //     return false;
        // }else
        //     outBuffer_ += "file open success!\r\n";
        // outBuffer_ += line_.version; 
        
        //解析请求头
        outBuffer_ += "Connection : " + head_.Connection + "\r\n";
        outBuffer_ += "HOST" + head_.Host + "\r\n";
        outBuffer_ += "Accept" + head_.Accept + "\r\n";
        outBuffer_ += "Content-Type" + head_.contentType + "\r\n";
        
        string fileName = line_.url;
        struct stat fileInfo;
        if(stat(fileName.c_str(), fileInfo) < 0){
            doError();
            return false;
        }

        if(line_.method == "HEAD")
            return true;
        //解析请求体
        int fileFd = open(fileName, O_WRONLY);
        if(fileFd < 0){
			outBuffer_.clear();
			//doError(fd_, 404, "Not Found!");
            doError("404");
			return false;  
        }
		//文件映射
        void *mmapRet = mmap(NULL, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fileFd, 0);
		close(fileFd);
        if(mmapRet == (void *)-1)
		{
			munmap(mmapRet, fileInfo.st_size);
			outBuffer_.clear();
			//doError(fd_, 404, "Not Found!");
            doError("404");
			return false;
		}
        char *src_addr = static_cast<char*>(mmapRet);
		//读取映射段
        outBuffer_ += string(src_addr, src_addr + fileInfo.st_size);;
        munmap(mmapRet, fileInfo.st_size);
        return true;

	}
    // else if(line_.method == "POST"){
	// 	outBuffer_ += "This method comes with 'POST'";
	// }

	return false;
}

// void httpData::newEvent()
// {
// 	spChannel->setEvents(EPOLLIN | EPOLLET);
// 	loop_->addToPoller(spChannel, 0);
// }