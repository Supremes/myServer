#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <assert.h>
#include <fcntl.h>
#include <vector>
#include <time.h>
#include <sys/time.h>
//引用失败
//#include "../base/utility.h"
using namespace std;

const int port = 4444;
const char* ip = "127.0.0.1";

int setSocketNonBlocking1(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1)
        return -1;

    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) == -1)
        return -1;
    return 0;
}
// string getNowTime()
// {
//     timespec time;
//     clock_gettime(CLOCK_REALTIME, &time);  //获取相对于1970到现在的秒数
//     struct tm nowTime;
//     localtime_r(&time.tv_sec, &nowtime);
//     char current[1024];
//     sprintf(current, "%04d%02d%02d%02d:%02d:%02d", nowTime.tm_year + 1900, nowTime.tm_mon+1, nowTime.tm_mday, 
//         nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
//     return string(current);
// }

int main(int argc, char const *argv[])
{
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);
    int sockfd[5];
    for(int i = 0; i < 5; i++){
        sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);
        assert(sockfd[i] > 0);
    }

    vector<string> requestHead;
    requestHead.push_back("Content-Type: text/plain\r\n");
    requestHead.push_back("Host: 127.0.0.1:4444\r\n");
    requestHead.push_back("Connection: Keep-Alive\r\n");
    requestHead.push_back("Date: 2019-02-26\r\n");
    requestHead.push_back("Accept-Language: En,Zh\r\n");
    int numOfRead = 0;
    char recv[100];
    int i = 0;
    while(i < 5){
        if(connect(sockfd[i], (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0){
            string send = "GET /home/supreme/djk/myServer/myServer/test.md HTTP/1.0\r\n" + requestHead[i];
            setSocketNonBlocking1(sockfd[i]);
            cout << "send data = " << send;
            cout << "sockfd = " << sockfd[i] << endl;
            write(sockfd[i], send.c_str(), send.size());
            sleep(1);
            while((numOfRead = read(sockfd[i], recv, 100)) == -1);
            cout << "numOfRead = " << numOfRead << endl;
            cout << recv << endl;
            //为什么服务器端依然保持该连接???,断开失败
            close(sockfd[i]);
        }
        i++;
    }


    // sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // assert(sockfd > 0);
    // if(connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0){
    //     char send[1000] = "images/t1.jpeg images/t2.jpeg", recv[1000] = "\0";
    //     setSocketNonBlocking1(sockfd);
    //     write(sockfd, send, strlen(send));
    //     int numOfRead;
    //     sleep(1);
    //     numOfRead = read(sockfd, recv, 1000);
    //     cout << "numOfRead = " << numOfRead << endl;
    //     cout << recv << endl;
    //     close(sockfd);
    // }
    return 0;
}
