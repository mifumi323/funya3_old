//	yaneIntervalTimer.cpp
#include "stdafx.h"
#include "yaneIntervalTimer.h"
#include "yaneAppInitializer.h"
#include "yaneTimer.h"

//////////////////////////////////////////////////////////////////////////////
//
//	マルチメディアタイマ
//
//////////////////////////////////////////////////////////////////////////////

CIntervalTimer::CIntervalTimer(void){
	m_hTimer = NULL;	//	マルチメディアタイマ
	m_uTimer = 0;		//　通常のタイマ
	m_dwIntervalMin = CTimeBase::timeBeginPeriodMin();
	m_bStop	 = true;
}

CIntervalTimer::~CIntervalTimer(){
	Stop();
	CTimeBase::timeEndPeriodMin();
}

bool	CIntervalTimer::IsTimer(void) const {
	return m_hTimer!=NULL || m_uTimer!=0 || !m_bStop;
}

LRESULT CIntervalTimer::Start(DWORD dwInterval,bool bMultiMediaTimer){
	Stop();

	if (!bMultiMediaTimer || dwInterval >= 20) {	//	インターバルが甘いなら通常タイマでいいでしょう？
		//	通常のタイマで済ませる場合
		m_uTimer = ::SetTimer(CAppInitializer::GetHWnd(),(UINT)this,dwInterval,(TIMERPROC)StaticTimerProc);
																			// ↑このキャストをしないとVC++5で通らないのだ＾＾；；
		if (m_uTimer==0) return 1;	//	error..
	} else {
		//	サウンドカードが付いていれば、通例、この値は1だと考えて良いはずだが...
		if (m_dwIntervalMin > dwInterval) return 1; // だめじゃん...
		//	delayを厳しくするとCPU負荷が上がるので...
		DWORD delay = dwInterval*90/100;	// 90%のずれまで許容する
		if (delay==0) delay++;
		m_hTimer = ::timeSetEvent(delay,dwInterval,StaticTimeProc,(DWORD)this,TIME_PERIODIC);
		if (m_hTimer==NULL) return 2;	//	timeSetEventで失敗...
	}
	return 0;
}

//	こちらはワンショットタイマで擬似的にインターバルタイマを実現する
//	こうすることによって、タイマコールバック中に、次のタイマ待ちになって
//	CPU負荷が極端に上がることは無くなる。
LRESULT CIntervalTimer::StartM(DWORD dwInterval){
	Stop();

	//	サウンドカードが付いていれば、通例、この値は1だと考えて良いはずだが...
	if (m_dwIntervalMin > dwInterval) return Start(dwInterval); // だめじゃん...
	//	delayを厳しくするとCPU負荷が上がるので...
	
	m_dwDelay = dwInterval*90/100;	// 90%のずれまで許容する
	if (m_dwDelay==0) m_dwDelay++;

	//	インターバルタイマをコピーしておく
	m_dwInterval = dwInterval;
	
	//	ワンショットタイマーを起動
	m_bStop = false;
	m_hTimer = ::timeSetEvent(m_dwDelay,m_dwInterval,StaticTimeProcM,(DWORD)this,TIME_ONESHOT);
	if (m_hTimer==NULL) {
		m_bStop = true;
		return 2;	//	timeSetEventで失敗...
	}

	return 0;
}

LRESULT CIntervalTimer::Stop(void){
	m_bStop = true;
	if (m_uTimer!=0){
		::KillTimer(NULL,m_uTimer);
		m_uTimer = 0;
	}
	if(m_hTimer!=NULL) {
		::timeKillEvent(m_hTimer);
		m_hTimer = NULL;
	}
	return 0;
}

//	通常のインターバルタイマ用
void CALLBACK CIntervalTimer::StaticTimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime){
	CIntervalTimer*	lpTimer = (CIntervalTimer*)idEvent;
	if(lpTimer->m_uTimer!=0){	//	自分が掛けた割り込みか？（のはずなんだけど:p）
		lpTimer->TimerProc();
	} else {
	//	これは、hwndのメッセージディスパッチルーチンから呼び出されるため、
	//	Stopのあとも呼び出される可能性があるので、それらは無視して良い。
	}
}

//	マルチメディアタイマ用
void CALLBACK CIntervalTimer::StaticTimeProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2){
	CIntervalTimer*	lpTimer = (CIntervalTimer*)dwUser;
	if(lpTimer->m_hTimer == uID){	//	自分が掛けた割り込みか？（のはずなんだけど:p）
		lpTimer->TimerProc();
	} else {
//		Stopのあとも呼び出される可能性があるので、それらは無視して良い。
//		WARNING(true,"CIntervalTimer::StaticTimeProcで不明な呼び出し");
	}
}

//	マルチメディアタイマ用
void CALLBACK CIntervalTimer::StaticTimeProcM(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2){
	CIntervalTimer*	lpTimer = (CIntervalTimer*)dwUser;
	if (lpTimer->m_hTimer == uID){	//	自分が掛けた割り込みか？（のはずなんだけど:p）
		if (lpTimer->m_bStop) return ;	//	もう止まってるやん＾＾；
		lpTimer->TimerProc();
		//	Dual CPUなら、このTimerProcを実行している最中にもm_bStopフラグが更新され得る。
		//	if (lpTimer->m_bStop) return ;	//	もう止まってるやん＾＾；

		//	次のタイマーかけやな
		lpTimer->m_hTimer = ::timeSetEvent(lpTimer->m_dwDelay,lpTimer->m_dwInterval,StaticTimeProcM,dwUser,TIME_ONESHOT);
	} else {
	//	Stopのあとも呼び出される可能性があるので、それらは無視して良い。
//		WARNING(true,"CIntervalTimer::StaticTimeProcMで不明な呼び出し");
	}
}