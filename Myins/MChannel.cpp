#include "MChannel.h"
#include<stdlib.h>
#include <iostream>
#include<unistd.h>
#include <fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<algorithm>
using namespace std;
bool StdinChannel::init()
{

	return true;
}

string StdinChannel::ReadFd()
{
	string str;
	cin >> str;
	return string(str);
}

bool StdinChannel::WriteFd(string _ouput)
{
	return false;
}

int StdinChannel::GetFd()
{
	return 0;
}


void StdinChannel::fini()
{
}

Mhandler* StdinChannel::GetNextStage(MyinsMsg* _input)
{
	return m_out;
}


bool StdOutChannel::init()
{
	return true;
}

string StdOutChannel::ReadFd()
{
	return string();
}

bool StdOutChannel::WriteFd(string _ouput)
{
	cout << _ouput << endl;
	return true;
}

int StdOutChannel::GetFd()
{
	return 1;
}


void StdOutChannel::fini()
{
}

Mhandler* StdOutChannel::GetNextStage(MyinsMsg* _input)
{
	return nullptr;
}

FIFOChannel::FIFOChannel(string fn, bool flag, MChannel *out) :filename(fn), isRead(flag), m_out(out)
{

}

bool FIFOChannel::init()
{
	//���ݹܵ�������ļ�
	int flag = O_RDONLY;
	if (false == isRead) {
		flag = O_WRONLY;
	}
	int fd = open(filename.c_str(), flag | O_NONBLOCK);
	if (fd < 0) {
		printf("open erro\n");
		return false;
	}
	m_fd = fd;
	return true;
}

string FIFOChannel::ReadFd()
{
	//��ȡ����
	char buffer[1024] = { 0 };
	int len = read(m_fd, buffer, sizeof(buffer));
	return string(buffer, len);
}

bool FIFOChannel::WriteFd(string _ouput)
{
	//��string����������������д��
	char *buffer = (char*)malloc(_ouput.size());
	_ouput.copy(buffer, _ouput.size(), 0);
	int ret = write(m_fd, buffer, _ouput.size());
	free(buffer);
	if (ret < 0) {
		printf("write erro\n");
		return false;
	}
	return true;
}

int FIFOChannel::GetFd()
{
	return m_fd;
}


void FIFOChannel::fini()
{
	//�رչܵ�
	close(m_fd);
}

Mhandler* FIFOChannel::GetNextStage(MyinsMsg* _input)
{
	return m_out;
}


void MChannel::DataSendOut(string _ouput)
{
	//��¼����������
	m_buffer.append(_ouput);
	//�޸�epoll�������ͣ����out
	MyinsKernel::GetInstance()->ModChannel_AddOut(this);
}

void MChannel::FlushOut()
{
	WriteFd(m_buffer);
	m_buffer.clear();
}

MyinsMsg* MChannel::InternelHandle(MyinsMsg* _input)
{
	auto pmsg = dynamic_cast<SysIOMsg*>(_input);
	if (NULL != pmsg) {
		//������������ȡ
		if (pmsg->dic == SysIOMsg::IN) {
			auto content = ReadFd();
			return new ByteMsg(content, *pmsg);
		}
		else {
			//������������������ݣ����ı�epoll
			auto pret = dynamic_cast<ByteMsg*>(_input);
			if (NULL != pret) {
				DataSendOut(pret->content);
			}
		}
	}
	return nullptr;
}

Mhandler* MChannel::GetNext(MyinsMsg* _input)
{
	auto pmsg = dynamic_cast<ByteMsg*>(_input);
	if (NULL != pmsg) {
		return GetNextStage(pmsg);
	}
	return nullptr;
}

void dataProcess::toup(string &input)
{
	transform(input.begin(), input.end(), input.begin(), ::toupper);
}


MyinsMsg* dataProcess::InternelHandle(MyinsMsg* _input)
{
	auto pmsg = dynamic_cast<ByteMsg*>(_input);
	if (NULL != pmsg) {
		if ('a' <= pmsg->content[0] && pmsg->content[0] <= 'z') {
			toup(pmsg->content);
		}
		//����send
		MyinsKernel::GetInstance()->KernelSendOut(m_out, pmsg->content);
	}
	return nullptr;
}

Mhandler* dataProcess::GetNext(MyinsMsg* _input)
{
	return nullptr;
}
