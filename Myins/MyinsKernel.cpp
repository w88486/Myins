#include "MyinsKernel.h"
#include<unistd.h>
MyinsKernel* MyinsKernel::kernel = new MyinsKernel();
MyinsKernel::MyinsKernel() {
	int fd = epoll_create(1);
	if (fd > 0) {
		epoll_fd = fd;
	}
}
void MyinsKernel::run()
{
	//循环持续执行
	while (1) {
		struct epoll_event events[200];
		//等输入
		int ret_size = epoll_wait(epoll_fd, events, 200, -1);
		if (0 == ret_size) {
			if (EINTR == errno) {
				continue;
			}
		}
		//调用通道执行操作
		for (int i = 0; i < ret_size; ++i) {
			//需要处理输入
			if (0 != (events[i].events & EPOLLIN)) {
				auto in = static_cast<MChannel*>(events[i].data.ptr);
				SysIOMsg* pmsg = new SysIOMsg(SysIOMsg::IN);
				in->Handle(pmsg);
			}
			//需要输出,刷新缓存
			if (0 != (events[i].events & EPOLLOUT)) {
				auto out = static_cast<MChannel*>(events[i].data.ptr);
				out->FlushOut();
				ModChannel_DelOut(out);
			}
		}
	}
}

bool MyinsKernel::AddChannel(MChannel* channel)
{
	//初始化管道
	if (true == channel->init()) {
		struct epoll_event event;
		event.events = EPOLLIN;
		event.data.ptr = channel;
		//将参数和描述符0关联起来
		int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, channel->GetFd(), &event);
		if (ret < 0) {
			return false;
		}
		return true;
	}
}

void MyinsKernel::DelChannel(MChannel* channel)
{
	//将参数和描述符0取消关联起来
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, channel->GetFd(), NULL);
	channel->fini();
}

bool MyinsKernel::ModChannel_AddOut(MChannel* channel)
{
		struct epoll_event event;
		event.events = EPOLLIN | EPOLLOUT;
		event.data.ptr = channel;
		//将参数和描述符0关联起来
		int ret = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, channel->GetFd(), &event);
		if (ret < 0) {
			return false;
		}
		return true;
}

void MyinsKernel::ModChannel_DelOut(MChannel* channel)
{
	//将参数和描述符1取消关联起来
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.ptr = channel;
	//将参数和描述符0关联起来
	int ret = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, channel->GetFd(), &event);
}

void MyinsKernel::KernelSendOut(MChannel* channel, string Msg)
{
	SysIOMsg iodic(SysIOMsg::OUT);
	channel->Handle(new ByteMsg(Msg, iodic));
}

MyinsKernel::~MyinsKernel()
{
	close(epoll_fd);
}
