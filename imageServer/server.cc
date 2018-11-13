#include "server.h"
#include "base/utility.h"
#include "imageData.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

server::server(EventLoop *loop, int port, int threadnum):
				loop_(loop),
				pool_(new EventLoopThreadPool(loop_, threadnum)),
				port_(port),
				listenfd_(socket_bind_listen(port_)),
				acceptChannel_(new Channel(loop_, listenfd_)),
				threadnum_(threadnum)
{
	if(setSocketNonBlocking(listenfd_) < 0)
		perror("listenfd_ set non block failed!");
	//handle_for_sigpipe();
}
void server::start()
{
	//pool_->start();
	acceptChannel_->setEvents(EPOLLIN | EPOLLET);
	acceptChannel_->setReadResponse(bind(&server::handleNewConnection, this));
	acceptChannel_->setConnResponse(bind(&server::handleThisConnection, this));

	cout << "listenfd_ = " << listenfd_ << endl;
	
	loop_->addToPoller(acceptChannel_, 0);
}
void server::handleNewConnection()
{
	cout << "server::handleNewConnection()" << endl;

	struct sockaddr_in client_addr = { 0 };
	socklen_t client_addr_len = sizeof client_addr;
	cout << "listenfd_ = " << listenfd_ << endl;
	int accept_fd = 0;
	while((accept_fd = accept(listenfd_, (struct sockaddr*)&client_addr, &client_addr_len)) > 0){
		//EventLoop *curLoop = pool_->getNextLoop();
		cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;
		cout << "accept_fd : " << accept_fd << endl;
		// 限制服务器的最大并发连接数
        // if (accept_fd >= MAXFDS)
        // {
        //     close(accept_fd);
        //     continue;
        // }

		if(setSocketNonBlocking(accept_fd) < 0){
			perror("set non block failed!");
		}
		
		setSocketNodelay(accept_fd);
		shared_ptr<imageData> accept_http(new imageData(loop_, accept_fd));
		//accept_http->getChannel()->setHolder(accept_http);
		loop_->runInLoop(bind(&imageData::handleNewEvent, accept_http));	//加入到pendingFuntors
	}

	acceptChannel_->setEvents(EPOLLIN | EPOLLET);

}

