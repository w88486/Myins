#pragma once
#include<string>
#include"MyinsKernel.h"
#include"Mhandler.h"
using namespace std;
class MChannel : public Mhandler
{
public:
	string m_buffer;
	virtual bool init() = 0;
	virtual string ReadFd() = 0;
	virtual bool WriteFd(string _ouput) = 0;
	virtual int GetFd() = 0;
	void DataSendOut(string _ouput);
	void FlushOut();
	virtual void fini() = 0;
	//管道下一个处理过程，需要重写
	virtual Mhandler *GetNextStage(MyinsMsg* _input) = 0;

	// 通过 Mhandler 继承
	MyinsMsg* InternelHandle(MyinsMsg* _input) override;
	Mhandler* GetNext(MyinsMsg* _input) override;
};
class StdinChannel :public MChannel {
	Mhandler *m_out = NULL;
public:
	StdinChannel(Mhandler* out) :m_out(out) {};
	// 通过 MChannel 继承
	bool init() override;
	string ReadFd() override;
	bool WriteFd(string _ouput) override;
	int GetFd() override;
	void fini() override;

	// 通过 MChannel 继承
	Mhandler* GetNextStage(MyinsMsg* _input) override;
};
class StdOutChannel :public MChannel {
public:
	// 通过 MChannel 继承
	bool init() override;
	string ReadFd() override;
	bool WriteFd(string _ouput) override;
	int GetFd() override;
	void fini() override;


	// 通过 MChannel 继承
	Mhandler* GetNextStage(MyinsMsg* _input) override;

};
class FIFOChannel :public MChannel {
	int m_fd = -1;
	bool isRead = true;
	string filename;
	MChannel* m_out = NULL;
public:
	FIFOChannel(string fn, bool flag, MChannel *out);
	// 通过 MChannel 继承
	bool init() override;
	string ReadFd() override;
	bool WriteFd(string _ouput) override;
	int GetFd() override;
	void fini() override;


	// 通过 MChannel 继承
	Mhandler* GetNextStage(MyinsMsg* _input) override;
};
class dataProcess :public Mhandler {
public:
	MChannel* m_out = NULL;
	void toup(string &input);
	// 通过 Mhandler 继承
	MyinsMsg* InternelHandle(MyinsMsg* _input) override;
	Mhandler* GetNext(MyinsMsg* _input) override;
};

