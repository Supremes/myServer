#include "EventLoop.h"
#include "Channel.h"
#include "server.h"
#include <iostream>
using namespace std;

int main(int argc, char const *argv[])
{
    int port = 6666;
    if(argc == 2)
        port = atoi(argv[1]);
    EventLoop *mainLoop = new EventLoop();
    server imageServer(mainLoop, port, 4);
    imageServer.start();
    //int listenFd = imageServer.getListenFd_();
    mainLoop->loop();
    return 0;
}
