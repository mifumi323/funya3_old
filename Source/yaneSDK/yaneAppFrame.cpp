#include "stdafx.h"
#include "yaneAppFrame.h"
#include "yaneTimer.h"

void	CAppFrame::MesSleep(int nTime) {
	CTimer t;
	while (IsThreadValid() && t.Get()<nTime)
		::Sleep(10);	//	����҂��Ƃ��O�O�G
}

//	�R���X�g���N�^�`�f�X�g���N�^�ŁACAppManager�ɓo�^���Ă���
CAppFrame::CAppFrame(void) {
	CAppManager::Add(this);
}

CAppFrame::~CAppFrame(){
	CAppManager::Del(this);
}