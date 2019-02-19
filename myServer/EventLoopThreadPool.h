#pragma once
#include "EventLoopThread.h"
#include "EventLoop.h"
#include <memory>
#include <vector>

using namespace std;

class EventLoopThreadPool
{
public:
    EventLoopThreadPool(EventLoop* baseLoop, int numThreads);
    ~EventLoopThreadPool();
    void start();

    EventLoop *getNextLoop();

private:
    EventLoop* baseLoop_;
    bool started_;
    int numThreads_;
    int next_;
    vector<shared_ptr<EventLoopThread>> threads_;
    vector<EventLoop*> loops_;
};