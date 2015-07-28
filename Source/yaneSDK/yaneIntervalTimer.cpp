//	yaneIntervalTimer.cpp
#include "stdafx.h"
#include "yaneIntervalTimer.h"
#include "yaneAppInitializer.h"
#include "yaneTimer.h"

//////////////////////////////////////////////////////////////////////////////
//
//	�}���`���f�B�A�^�C�}
//
//////////////////////////////////////////////////////////////////////////////

CIntervalTimer::CIntervalTimer(void){
	m_hTimer = NULL;	//	�}���`���f�B�A�^�C�}
	m_uTimer = 0;		//�@�ʏ�̃^�C�}
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

	if (!bMultiMediaTimer || dwInterval >= 20) {	//	�C���^�[�o�����Â��Ȃ�ʏ�^�C�}�ł����ł��傤�H
		//	�ʏ�̃^�C�}�ōς܂���ꍇ
		m_uTimer = ::SetTimer(CAppInitializer::GetHWnd(),(UINT)this,dwInterval,(TIMERPROC)StaticTimerProc);
																			// �����̃L���X�g�����Ȃ���VC++5�Œʂ�Ȃ��̂��O�O�G�G
		if (m_uTimer==0) return 1;	//	error..
	} else {
		//	�T�E���h�J�[�h���t���Ă���΁A�ʗ�A���̒l��1���ƍl���ėǂ��͂�����...
		if (m_dwIntervalMin > dwInterval) return 1; // ���߂����...
		//	delay�������������CPU���ׂ��オ��̂�...
		DWORD delay = dwInterval*90/100;	// 90%�̂���܂ŋ��e����
		if (delay==0) delay++;
		m_hTimer = ::timeSetEvent(delay,dwInterval,StaticTimeProc,(DWORD)this,TIME_PERIODIC);
		if (m_hTimer==NULL) return 2;	//	timeSetEvent�Ŏ��s...
	}
	return 0;
}

//	������̓����V���b�g�^�C�}�ŋ[���I�ɃC���^�[�o���^�C�}����������
//	�������邱�Ƃɂ���āA�^�C�}�R�[���o�b�N���ɁA���̃^�C�}�҂��ɂȂ���
//	CPU���ׂ��ɒ[�ɏオ�邱�Ƃ͖����Ȃ�B
LRESULT CIntervalTimer::StartM(DWORD dwInterval){
	Stop();

	//	�T�E���h�J�[�h���t���Ă���΁A�ʗ�A���̒l��1���ƍl���ėǂ��͂�����...
	if (m_dwIntervalMin > dwInterval) return Start(dwInterval); // ���߂����...
	//	delay�������������CPU���ׂ��オ��̂�...
	
	m_dwDelay = dwInterval*90/100;	// 90%�̂���܂ŋ��e����
	if (m_dwDelay==0) m_dwDelay++;

	//	�C���^�[�o���^�C�}���R�s�[���Ă���
	m_dwInterval = dwInterval;
	
	//	�����V���b�g�^�C�}�[���N��
	m_bStop = false;
	m_hTimer = ::timeSetEvent(m_dwDelay,m_dwInterval,StaticTimeProcM,(DWORD)this,TIME_ONESHOT);
	if (m_hTimer==NULL) {
		m_bStop = true;
		return 2;	//	timeSetEvent�Ŏ��s...
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

//	�ʏ�̃C���^�[�o���^�C�}�p
void CALLBACK CIntervalTimer::StaticTimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime){
	CIntervalTimer*	lpTimer = (CIntervalTimer*)idEvent;
	if(lpTimer->m_uTimer!=0){	//	�������|�������荞�݂��H�i�̂͂��Ȃ񂾂���:p�j
		lpTimer->TimerProc();
	} else {
	//	����́Ahwnd�̃��b�Z�[�W�f�B�X�p�b�`���[�`������Ăяo����邽�߁A
	//	Stop�̂��Ƃ��Ăяo�����\��������̂ŁA�����͖������ėǂ��B
	}
}

//	�}���`���f�B�A�^�C�}�p
void CALLBACK CIntervalTimer::StaticTimeProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2){
	CIntervalTimer*	lpTimer = (CIntervalTimer*)dwUser;
	if(lpTimer->m_hTimer == uID){	//	�������|�������荞�݂��H�i�̂͂��Ȃ񂾂���:p�j
		lpTimer->TimerProc();
	} else {
//		Stop�̂��Ƃ��Ăяo�����\��������̂ŁA�����͖������ėǂ��B
//		WARNING(true,"CIntervalTimer::StaticTimeProc�ŕs���ȌĂяo��");
	}
}

//	�}���`���f�B�A�^�C�}�p
void CALLBACK CIntervalTimer::StaticTimeProcM(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2){
	CIntervalTimer*	lpTimer = (CIntervalTimer*)dwUser;
	if (lpTimer->m_hTimer == uID){	//	�������|�������荞�݂��H�i�̂͂��Ȃ񂾂���:p�j
		if (lpTimer->m_bStop) return ;	//	�����~�܂��Ă���O�O�G
		lpTimer->TimerProc();
		//	Dual CPU�Ȃ�A����TimerProc�����s���Ă���Œ��ɂ�m_bStop�t���O���X�V���꓾��B
		//	if (lpTimer->m_bStop) return ;	//	�����~�܂��Ă���O�O�G

		//	���̃^�C�}�[�������
		lpTimer->m_hTimer = ::timeSetEvent(lpTimer->m_dwDelay,lpTimer->m_dwInterval,StaticTimeProcM,dwUser,TIME_ONESHOT);
	} else {
	//	Stop�̂��Ƃ��Ăяo�����\��������̂ŁA�����͖������ėǂ��B
//		WARNING(true,"CIntervalTimer::StaticTimeProcM�ŕs���ȌĂяo��");
	}
}