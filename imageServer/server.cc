#include "server.h"
#include "base/utility.h"
#include "imageData.h"
#include <iostream>
using namespace std;

server::server(EventLoop *loop, int port, int threadnum):
				loop_(loop),
				port_(port),
				listenfd_(socket_bind_listen(port_)),
				acceptChannel_(new Channel(loop_, listenfd_)),
				threadnum_(threadnum)
{

}
void server::start()
{
	acceptChannel_->setEvents(EPOLLIN | EPOLLET);
	//为什么将新连接设置读回调，现有的连接设置为连接回调
	acceptChannel_->setReadResponse(bind(&server::handleNewConnection, this));
	acceptChannel_->setConnResponse(bind(&server::handleThisConnection, this));

	cout << "listenfd_ = " << listenfd_ << endl;
	
	loop_->addToPoller(acceptChannel_, 0);
}
void server::handleNewConnection()
{
	setSocketNonBlocking(listenfd_);
	struct sockaddr_in client_addr = { 0 };
	socklen_t client_addr_len = sizeof client_addr;

	int accpet_fd = 0;
	while((accpet_fd = accept(listenfd_, (struct sockaddr*)&client_addr, &client_addr_len)) > 0){
		//EventLoop *loop = eventLoopThread_.getNextLoop();
		if(setSocketNonBlocking(accpet_fd) < 0){
			perror("set non block failed!");
		}

		shared_ptr<imageData> accept_http(new imageData(loop_, accpet_fd));
		//accept_http->getChannel()->setHolder(accept_http);
		loop_->queueInLoop(bind(&imageData::handleNewEvent, accept_http));
	}

	acceptChannel_->setEvents(EPOLLIN | EPOLLET);

}

