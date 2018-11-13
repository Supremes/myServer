#pragma once
#include <string>
#include "Channel.h"
#include "EventLoop.h"
#include <unistd.h>

using namespace std;

class imageData{
public:
    imageData(EventLoop *loop, int connfd);
    ~imageData(){  cout << "imageData destructing!\n"; close(connfd_); }

    void stitch();

    void handleWrite();
    void handleRead();
    void handleError();
    void handleClose();
    
    void handleNewEvent();
private:
    vector<string> names_;

    string feature_;
    string warp_;
    string seam_;

    string inBuffer_;
    string outBuffer_;

    bool error_;

    int connfd_;
    EventLoop *loop_;
    spChannel channel_;
};

