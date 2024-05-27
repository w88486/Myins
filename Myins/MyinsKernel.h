#pragma once
#include<list>
#include<sys/epoll.h>
#include"MChannel.h"
#include"Mhandler.h"
class MChannel;
using namespace std;
class MyinsKernel
{
	int epoll_fd = -1;
	static MyinsKernel* kernel;
	list<MChannel*>channelList;
	MyinsKernel();
public:
	void run();
	bool AddChannel(MChannel *channel);
	void DelChannel(MChannel* channel);
	bool ModChannel_AddOut(MChannel* channel);
	void ModChannel_DelOut(MChannel* channel);
	void KernelSendOut(MChannel *channel, string Msg);
	static MyinsKernel* GetInstance() {
		return kernel;
	}
	virtual ~MyinsKernel();
};

