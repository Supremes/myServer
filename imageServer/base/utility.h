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
#include <sys/syscall.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <memory>
#include <thread>

using namespace std;

int setSocketNonBlocking(int& fd);
int socket_bind_listen(int port);
ssize_t readn(int fd, string &buf, bool &zero);
ssize_t writen(int fd, string &buf);
inline int djk_is_file_existed(const char *path);
void splitString(const string &s, const string &p, vector<string> &result);
pid_t gettid();