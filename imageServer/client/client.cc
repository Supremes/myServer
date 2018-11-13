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

//引用失败
//#include "../base/utility.h"
using namespace std;

const int port = 6666;
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
int main(int argc, char const *argv[])
{
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);
    if(connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){
        perror("can't connect server");
    }
    if(setSocketNonBlocking1(sockfd))
        perror("sockfd set nonblock failed!");
    char send[1000] = "images/s1.jpg images/s2.jpg", recv[1000];
    // while(fgets(send, 1000, stdin) != NULL){
    write(sockfd, send, strlen(send));
    while(true){
        int numOfRead;
        if((numOfRead = read(sockfd, recv, 1000)) > 0))
        {
            cout << "read something:" << endl;
            fputs(recv, stdout);
        }
        else if(numOfRead == 0)
            continue;
        else{
            perror("server terminated");
            break;
        }
    }
    return 0;
}
