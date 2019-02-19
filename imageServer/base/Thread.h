#pragma once
#include <pthread.h>
#include <functional>
#include <assert.h>

using namespace std;

class ThreadData{
public:
	typedef function<void()> threadFunc;
	ThreadData(const threadFunc& func);
	void run();
private:
	string name_;
	int id_;
	threadFunc func_;
};

class Thread{
public:
	typedef function<void()> threadFunc;
	Thread(const threadFunc& func);
	~Thread();
	int run();
	int join();
	bool isRunning();
	pthread_t getId();
private:
	pthread_t thread_id;
	bool running;
	bool joined;
	threadFunc func_;
	ThreadData *data;
};
