#pragma once
#include <string>
#include "Channel.h"
#include "EventLoop.h"
#include <unistd.h>

using namespace std;

class imageData{
public:
    imageData(EventLoop *loop, int connfd);
    ~imageData();
    
    void handleNewEvent();
    //关系到timerNode处理关闭事件,应设为public
    void handleClose();
    spChannel getChannel()  { return channel_; }
    int getFd(){ return connfd_; }
    EventLoop* getLoop() { return loop_; }
    void connectDestroyed();
private:
	enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    void setState(StateE state) { connState_ = state; }
    void stitch();
    
    void handleWrite();
    void handleRead();
    void handleError();
	
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
    StateE connState_;
};

typedef shared_ptr<imageData> imageDataPtr;
