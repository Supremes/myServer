#include "timer.h"
#include "imageData.h"
#include <sys/time.h>
#include <iostream>

using namespace std;

timerNode::timerNode(shared_ptr<imageData> requestData, int timeout):
						deleted(false),spImageData(requestData),expiredTimer(timeout)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	expiredTimer = now.tv_sec * 1000 + now.tv_usec / 1000 + timeout;
}
timerNode::~timerNode()
{
	//回调imageData的关闭接口，处理imageData
	if(spImageData)
		spImageData->handleClose();
}

void timerNode::update(int timeout)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	expiredTimer = now.tv_sec * 1000 + now.tv_usec / 1000 + timeout;
}

void timerNode::clearRequests()
{
	spImageData.reset();
	this->setDeleted();
}

bool timerNode::isValid()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	size_t time = now.tv_sec * 1000 + now.tv_usec / 1000;
	if(time < expiredTimer)
		return true;
	else{
		this->setDeleted();
		return false;
	}
}

void timerNodeManager::addTimer(shared_ptr<imageData> spImageData, int timeout)
{
	spTimerNode temp(new timerNode(spImageData, timeout));
	timerNodeQueue.push(temp);
	//这一步着重看一下
	//spImageData->linkTimer(temp);
}

void timerNodeManager::handleExpiredEvent()
{
	while(!timerNodeQueue.empty()){
		spTimerNode tNode = timerNodeQueue.top();
		cout << "handleExpiredEvent..." << endl;
		if(tNode->isDeleted() == true)
			timerNodeQueue.pop();
		else if(tNode->isValid() == false)
			timerNodeQueue.pop();
		else
			break;
	}
	
}