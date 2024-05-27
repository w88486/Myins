#include<iostream>
#include"MyinsKernel.h"
#include"MChannel.h"
#include"Mhandler.h"
using namespace std;
int main() {
	auto out = new StdOutChannel();
	auto process = new dataProcess();
	process->m_out = out;
	auto in = new StdinChannel(process);
	MyinsKernel::GetInstance()->AddChannel(in);
	MyinsKernel::GetInstance()->AddChannel(out);
	/*auto fifoout = new FIFOChannel("fifoout", false, NULL);
	auto fifoin = new FIFOChannel("fifoin", true, fifoout);
	MyinsKernel::GetInstance()->AddChannel(fifoin);
	MyinsKernel::GetInstance()->AddChannel(fifoout);*/
	MyinsKernel::GetInstance()->run();
	return 0;
}