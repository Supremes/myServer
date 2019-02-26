#include "httpData.h"
#include "timer.h"

using namespace std;

//初始化列表的成员初始化顺序是按照变量的声明顺序的
httpData::httpData(EventLoop* loop, int connfd):
			connfd_(connfd),
			error_(false),
			keepAlive_(false),
			loop_(loop),
			spChannel(new Channel(loop_, connfd_)),
			state_(PARSE_LINE)

{

	spChannel->setReadResponse(bind(&httpData::handleRead, this));
	spChannel->setWriteResponse(bind(&httpData::handleWrite, this));
	//spChannel->setConnResponse(bind(&httpData::handleConn, this));

}

// string MimeType::getMime(const string& suffix)
// {
// 	//需要设置线程变量初始化的原因是什么？
// 	//pthread_once(&once_control, MimeType::init);
// 	if(mime.find(suffix) == mime.end())
// 		return mime["default"];
// 	else
// 		return mime[suffix];
// }
void httpData::seperateTimer()
{
	shared_ptr<timerNode> this_timer(timer_.lock());
	this_timer->clearRequests();
}
void httpData::handleClose()
{
	connectionState_ = HTTP_DISCONNECTED;
	//设置guard共享智能指针的原因是什么
	shared_ptr<httpData> guard(shared_from_this());
	loop_->removeFromPoller(spChannel);
	seperateTimer();
}

void httpData::handleRead()
{
	//uint32_t &events = spChannel->getEvents();
	//cout << "httpData::handleRead()\n";
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
		int index = inBuffer_.find_first_of("\r", 0);
		vector<string> parseString;
		parseString.push_back(inBuffer_.substr(0, index));
		parseString.push_back(inBuffer_.substr(index + 2, inBuffer_.size() - index - 2));
		//解析http字段的条件判断
		if(state_ == PARSE_LINE){
			if(parseLine(parseString[0]))
				state_ = PARSE_HEAD;
		}
		if(state_ == PARSE_HEAD){
			if(parseHeader(parseString[1]))
				state_ = PARSE_BODY;
		}
		if(state_ == PARSE_BODY){
			//根据httpData中的请求数据进行响应
			if(!doHttpData()){
				doError();
			}
		}

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
		//ET边缘触发需要设置,不然可能写事件可能没法及时触发
		// if(outBuffer_ > 0)
		// 	events_temp |= EPOLLOUT;
	}

}
void httpData::handleNewConn()
{
	//int events = EPOLLIN | EPOLLOUT;
	spChannel->setEvents(EPOLLIN | EPOLLOUT);
	//cout << "events = " << events << endl;
	loop_->addToPoller(spChannel, 0);
	connectionState_ = HTTP_CONNECTED;
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
		cout << "header[i] = " << headers[i] << endl;
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
        
        //解析请求头
		if(head_.Connection.size())
        	outBuffer_ += "Connection : " + head_.Connection + "\r\n";
		if(head_.Host.size())
        	outBuffer_ += "HOST" + head_.Host + "\r\n";
		if(head_.Agent.size())
        	outBuffer_ += "User-Agent" + head_.Agent + "\r\n";
		if(head_.contentType.size())
        	outBuffer_ += "Content-Type: " + head_.contentType + "\r\n";
		if(head_.Date.size())
			outBuffer_ += "Date: " + head_.Date + "\r\n";
        
        string fileName = line_.url;
        struct stat fileInfo;
		cout << "fileName = " << fileName << endl;
        if(stat(fileName.c_str(), &fileInfo) < 0){
            doError();
            return false;
        }

        if(line_.method == "HEAD")
            return true;
        //解析请求体
        int fileFd = open(fileName.c_str(), O_RDONLY);
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
        outBuffer_ += string(src_addr, src_addr + fileInfo.st_size) + "\n";
        munmap(mmapRet, fileInfo.st_size);
        return true;

	}
    else if(line_.method == "POST"){
		outBuffer_ += "This method comes with 'POST'";
		return true;
	}
	else
		return false;
}

// void httpData::newEvent()
// {
// 	spChannel->setEvents(EPOLLIN);
// 	loop_->addToPoller(spChannel, 0);
// }