#include "stdafx.h"
#include "yaneAppFrame.h"
#include "yaneTimer.h"

void	CAppFrame::MesSleep(int nTime) {
	CTimer t;
	while (IsThreadValid() && t.Get()<nTime)
		::Sleep(10);	//	ちょぴっとずつ＾＾；
}

//	コンストラクタ〜デストラクタで、CAppManagerに登録しておく
CAppFrame::CAppFrame(void) {
	CAppManager::Add(this);
}

CAppFrame::~CAppFrame(){
	CAppManager::Del(this);
}