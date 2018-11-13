#include "utility.h"

const int MAX_BUFF = 2048;

int setSocketNonBlocking(int fd)
{
	int flag = fcntl(fd, F_GETFL, 0);
	if(flag == -1){
		return -1;
    }
	flag |= O_NONBLOCK;
	if(fcntl(fd, F_SETFL, flag) == -1){
		return -1;
    }
	return 0;
}

inline int djk_is_file_existed(char *path){
	int fd = open(path, O_RDONLY | O_EXCL);
	close(fd);
	return fd;
}

int socket_bind_listen(int port)
{
	cout << "port = " << port << endl;
    // 检查port值，取正确区间范围
    if (port < 0 || port > 65535)
        return -1;

    // 创建socket(IPv4 + TCP)，返回监听描述符
    int listen_fd = 0;
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;

    // 消除bind时"Address already in use"错误
    int optval = 1;
    if(setsockopt(listen_fd, SOL_SOCKET,  SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        return -1;

    // 设置服务器IP和Port，和监听描述副绑定
    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short)port);
    if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        return -1;

    // 开始监听，最大等待队列长为LISTENQ
    if(listen(listen_fd, 2048) == -1)
        return -1;

    // 无效监听描述符
    if(listen_fd == -1)
    {
        close(listen_fd);
        return -1;
    }
    return listen_fd;
}

ssize_t readn(int fd, std::string &inBuffer, bool &zero)
{
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while (true)
    {
        char buff[MAX_BUFF];
        if ((nread = read(fd, buff, MAX_BUFF)) < 0)
        {
            if (errno == EINTR)
                continue;
            else if (errno == EAGAIN)
            {
                return readSum;
            }  
            else
            {
                perror("read error");
                return -1;
            }
        }
        else if (nread == 0)
        {
            zero = true;
            break;
        }
        readSum += nread;
        inBuffer += std::string(buff, buff + nread);
    }
    return readSum;
}
ssize_t readn(int fd, std::string &inBuffer)
{
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while (true)
    {
        char buff[MAX_BUFF];
        if ((nread = read(fd, buff, MAX_BUFF)) < 0)
        {
            if (errno == EINTR)
                continue;
            else if (errno == EAGAIN)
            {
                return readSum;
            }  
            else
            {
                perror("read error");
                return -1;
            }
        }
        else if (nread == 0)
        {
            //printf("redsum = %d\n", readSum);
            break;
        }
        //printf("before inBuffer.size() = %d\n", inBuffer.size());
        //printf("nread = %d\n", nread);
        readSum += nread;
        //buff += nread;
        inBuffer += std::string(buff, buff + nread);
        //printf("after inBuffer.size() = %d\n", inBuffer.size());
    }
    return readSum;
}
ssize_t writen(int fd, string &outBuffer){
	int resNum = 0;
	int writeNum = 0;
	int nleft = outBuffer.size();
	const char *ptr = outBuffer.c_str();
	while(nleft > 0){
		if((writeNum = write(fd, ptr, nleft)) < 0){
            if (errno == EINTR)
            {
                writeNum = 0;
                continue;
            }
            else if (errno == EAGAIN)
                break;
            else
                return -1;			
		}
		else if(writeNum == 0){
			break;
		}
		resNum += writeNum;
		ptr += writeNum;
		nleft -= writeNum;
	}
	return resNum;
}

pid_t gettid()
{
	return static_cast<pid_t>(syscall(SYS_gettid));
}
void splitString(const string &s, const string &p,
                vector<string> &result)
{
    std::string::size_type begin, end;
    begin = 0;
    end = s.find(p, begin);

    while (std::string::npos != end)
    {
        result.push_back(s.substr(begin, end - begin));

        begin = end + p.size();
        end = s.find(p, begin);
    }

    if (begin != s.size())
        result.push_back(s.substr(begin));
}

void setSocketNodelay(int fd) 
{
    int enable = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
}

void handle_for_sigpipe()
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, NULL))
        return;
}
// unique_ptr<int> thread_func()
// {
// 	unique_ptr<int> a(new int(2));
// 	*a += 1;
// 	cout << *a << endl;
// 	return a;
// }
// int main(int argc, char const *argv[])
// {
	// string requestHead = "Connection: Keep-Alive\r\nHost: 127.0.0.1:5555\r\n";
	// vector<string> res;
	// splitString(requestHead, "\r\n", res);
	// cout << res[0] << endl << res[1] << endl;
	// cout << "pos = " << requestHead.find("\n");
	// do{
	// 	cout << "in while" << endl;
	// }while(false);

	// shared_ptr<int> p1(new int(1));
	// {
	// 	shared_ptr<int> p2 = p1;
	// 	cout << "p1 count = " << p1.use_count() << endl;
	// 	cout << "p2 count = " << p2.use_count() << endl;
	// }
	// unique_ptr<int> p3(new int(2));
	// unique_ptr<int> p4 = move(p3);
	// cout << "p1 count = " << p1.use_count() << endl;
	// cout << "p4 = " << *p4 << endl;

	// unique_ptr<int> b = move(thread_func());
	// cout << *b << endl;

	//cout <<  "同时并发在一个程序中的线程数量: " << thread::hardware_concurrency() << endl;
//     int fd = open("1.txt", O_WRONLY, 0777);
//     cout << fd << endl;
//     string res;
//     int numOfRead = readn(fd, res);
//     cout << res << endl;
// 	return 0;
// }