#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <memory>
#include <thread>

using namespace std;

int setSocketNonBlocking(int fd);
int socket_bind_listen(int port);
ssize_t readn(int fd, string &buf, bool &zero);
ssize_t readn(int fd, std::string &inBuffer);
ssize_t writen(int fd, string &buf);
inline int djk_is_file_existed(const char *path);
void splitString(const string &s, const string &p, vector<string> &result);
pid_t gettid();
void setSocketNodelay(int fd);  //禁用Nagle算法，允许小包的发送
void handle_for_sigpipe();
void setSocketNoLinger(int fd); //优雅关闭