#include "Mhandler.h"

void Mhandler::Handle(MyinsMsg* _input)
{
	//执行当前操作
	MyinsMsg *pret = InternelHandle(_input);
	
	if (nullptr != pret) {
		//获取下一个步骤
		auto nextp = GetNext(pret);
		if (nullptr != nextp) {
			//执行下一个
			nextp->Handle(pret);
		}
		delete pret;
	}
}
