#include "server.h"
#include "base/utility.h"
#include "httpData.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
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
	pool_->start();
	acceptChannel_->setEvents(EPOLLIN);
	acceptChannel_->setReadResponse(bind(&server::handleNewConnection, this));
	acceptChannel_->setConnResponse(bind(&server::handleThisConnection, this));

	cout << "listenfd_ = " << listenfd_ << endl;
	
	loop_->addToPoller(acceptChannel_, 0);
}

server::~server()
{
	loop_->assertInLoopThread();
	for(ConnectionMap::iterator it = connections_.begin(); it != connections_.end(); it++){
		spHttpData conn(it->second);
		it->second.reset();
		conn->getLoop()->runInLoop(bind(&httpData::handleClose, conn));
	}
}
void server::handleNewConnection()
{
	cout << "server::handleNewConnection()" << endl;

	struct sockaddr_in client_addr = { 0 };
	socklen_t client_addr_len = sizeof client_addr;
	int accept_fd = 0;
	while((accept_fd = accept(listenfd_, (struct sockaddr*)&client_addr, &client_addr_len)) > 0){
		EventLoop *curLoop = pool_->getNextLoop();
		cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;
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
		setSocketNoLinger(accept_fd);
		setSocketNodelay(accept_fd);
		spHttpData accept_http(new httpData(curLoop, accept_fd));
		connections_[accept_fd] = accept_http;
		accept_http->getChannel()->setHolder(accept_http);
		curLoop->queueInLoop(bind(&httpData::handleNewConn, accept_http));	//加入到pendingFuntors
	}


}

// void server::removeConnectionInLoop(const spHttpData myConn)
// {
// 	loop_->assertInLoopThread();
// 	cout << "Server::removeConnectionInLoop - connection[fd ]  = " << myConn->getFd() << endl;
// 	size_t n = connections_.erase(myConn->getFd());
// 	(void)n;
// 	assert( n == 1);
// 	EventLoop *ioLoop = myConn->getLoop();
// 	ioLoop->queueInLoop(bind(&imageData::connectDestroyed, myConn));
// }

