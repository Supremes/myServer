#include "EventLoop.h"
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <unistd.h>
using namespace std;

__thread EventLoop* t_loopInThisThread = 0;

int createEventfd()
{
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        cout << "Failed in eventfd" << endl;
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop():
		looping_(false),
		threadId_(CurrentThread::tid()),
		wakeupfd_(createEventfd()),
		epoller_(new Epoller()),
		wakeupChannel_(new Channel(this, wakeupfd_)),
		callingPendingFunctors_(false),
		quit_(false)
{
	if(t_loopInThisThread){
	    cout << "Another EventLoop " << t_loopInThisThread << " exists in this thread " << threadId_ << endl;
	}else{
		t_loopInThisThread = this;
	}
	wakeupChannel_->setEvents(EPOLLIN);
	wakeupChannel_->setReadResponse(bind(&EventLoop::handleRead, this));
	epoller_->epoll_add(wakeupChannel_, 0);
}

EventLoop::~EventLoop()
{
	close(wakeupfd_);
	t_loopInThisThread = NULL;
}
//mind
void EventLoop::handleConn()
{	
	updatePoller(wakeupChannel_);
}

void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = read(wakeupfd_, &one, sizeof one);
	if (n != sizeof one)
	{	//等待编写LOG日志库
		//LOG << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
	}
}

void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = write(wakeupfd_, (char*)&one, sizeof one);
	if(n != sizeof one)
	{
		perror("wakeup failed!");
	}
}

void EventLoop::runInLoop(Functor&& function)
{
	if (isLoopInThread()){
		cout << "isLoopInThread..." << endl;
		function();
	}else{
		queueInLoop(std::move(function));
	}
}

void EventLoop::queueInLoop(Functor&& function)
{
	{
		MutexLockGuard lock(mutex_);
		pendingFunctors_.push_back(std::move(function));
	}

	if(!isLoopInThread() || callingPendingFunctors_)
		wakeup();
}

void EventLoop::doPendingFunctors()
{
	vector<Functor> functors;
	callingPendingFunctors_ = true;
	{
		MutexLockGuard lock(mutex_);
		functors.swap(pendingFunctors_);
	}
	for(size_t i = 0; i < functors.size(); i++)
		functors[i]();

	callingPendingFunctors_ = false;
	
}
void EventLoop::loop()
{
	assert(!looping_);
	assert(isLoopInThread());
	looping_ = true;
	//保存Epoller返回的channel列表
	vector<shared_ptr<Channel> > channelList;

	while(!quit_){
		channelList.clear();
		channelList = epoller_->poll();

		for(auto &ret: channelList){
			ret->handleEvents();
		}
		doPendingFunctors();
		epoller_->handleExpired();
	}
	
	looping_ = false;


}

void EventLoop::quit()
{
	quit_ = true;
	if(!isLoopInThread())
		wakeup();
}