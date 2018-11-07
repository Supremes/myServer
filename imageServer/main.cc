#include "EventLoop.h"
#include "Channel.h"
#include "server.h"
#include <iostream>
using namespace std;

int main(int argc, char const *argv[])
{
    int port = 5555;
    if(argc < 2){
        cout << "please input port" << endl;
    }
    else
        port = atoi(argv[1]);
    EventLoop *mainLoop = new EventLoop();
    server imageServer(mainLoop, port, 4);
    imageServer.start();
    mainLoop->loop();
    return 0;
}
