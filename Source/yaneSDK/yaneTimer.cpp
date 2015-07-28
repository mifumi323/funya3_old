//	yaneTimer.cpp
#include "stdafx.h"
#include "yaneTimer.h"

//////////////////////////////////////////////////////////////////////////////
int			CTimeBase::m_nRef = 0;			//	�Q�ƃJ�E���g
bool		CTimeBase::m_bUseTGT;
TIMECAPS	CTimeBase::m_dwTimeCaps;
//MMTIME		CTimeBase::m_mmtime;
CTimeBase	CTimeBaseInitializer;			//	�������N������CTimeBase::CTimeBase(void)���Ăяo��
CTimeBase __timebase_dummy;	// �R���X�g���N�^�ŏ��������邽�߂̂���

CTimeBase::CTimeBase(void) {
	if (::timeGetDevCaps(&m_dwTimeCaps,sizeof(TIMECAPS))!=TIMERR_NOERROR) {
		//	�Ȃ��H�^�C�}�����Ă�̂��H
		m_dwTimeCaps.wPeriodMin = 1;	//	�̂���ɂȂ�:p
		m_bUseTGT	= false;			// GetTickCount���g�p����
		return ;
	}
	if (m_dwTimeCaps.wPeriodMin>=10) {
		//	������傫���Ǝg�����̂ɂȂ��
		m_dwTimeCaps.wPeriodMin = 1;		//	�̂���ɂȂ�:p
		m_bUseTGT	= false;		// GetTickCount���g�p����
	} else {
		m_bUseTGT	= true;			// timeGetTime���g�p����
//		m_mmtime.wType = TIME_MS;	//	timeGetSystemTime
	}
}

//////////////////////////////////////////////////////////////////////////////

//	timeBeginPeriod��timeEndPeriod�͓�������\�ɂđΉ������邱�ƁB
//	�܂��A��������\�ɑ΂��Ă̓l�X�e�B���O�o���Ȃ��B�i�t�A�Z���Ē��ׂ��j

int CTimeBase::timeBeginPeriodMin(void){

	//---	�i�R���X�g���N�^�ł���Ă���̂Ɠ������e�j
	::timeGetDevCaps(&m_dwTimeCaps,sizeof(TIMECAPS));
	if (m_dwTimeCaps.wPeriodMin>=10) {
	//	������傫���Ǝg�����̂ɂȂ��
		m_dwTimeCaps.wPeriodMin = 1;		//	�̂���ɂȂ�:p
		m_bUseTGT	= false;		// GetTickCount���g�p����
	} else {
		m_bUseTGT	= true;			// timeGetTime���g�p����
//		m_mmtime.wType = TIME_MS;	//	timeGetSystemTime
	}
	//---

	if (m_nRef++==0 && m_bUseTGT) {
		// timeGetTime���g��Ȃ��Ȃ��timeBeginPeriod�͖��Ӗ�
		::timeBeginPeriod(m_dwTimeCaps.wPeriodMin);
	}
	return m_dwTimeCaps.wPeriodMin;
}

void CTimeBase::timeEndPeriodMin(void){
	if (--m_nRef==0 && m_bUseTGT) {
		::timeEndPeriod(m_dwTimeCaps.wPeriodMin);
	}
}

//////////////////////////////////////////////////////////////////////////////

set<CTimer*>	CTimer::m_aTimer;	//	�S�C���X�^���X�̃`�F�[��

CTimer::CTimer(void){
	m_aTimer.insert(this);	//	�`�F�C���ɃC���X�^���X�ǉ�
	Reset();
}

CTimer::~CTimer(){
	m_aTimer.erase(this);
}

DWORD CTimer::Get(void) {
	if (m_bPaused>0) return m_dwPauseTime-m_dwOffsetTime;
	return (DWORD)(CTimeBase::timeGetTime()-m_dwOffsetTime);
}

void CTimer::Set(DWORD dw) {
	if (m_bPaused>0) {
		//	����񂩂ȁ`�H
		m_dwOffsetTime = m_dwPauseTime-dw;
	} else {
		m_dwOffsetTime = (DWORD)(CTimeBase::timeGetTime() - dw);
	}
}

void CTimer::Reset(void) {
	m_dwOffsetTime = CTimeBase::timeGetTime();
	m_bPaused = 0;
}

void CTimer::Pause(void){
	if (m_bPaused++ == 0) {
	//	Pause������
		m_dwPauseTime = CTimeBase::timeGetTime();
	}
}

void CTimer::Restart(void){
	if (--m_bPaused == 0) {
	//	Pause����
		m_dwOffsetTime += CTimeBase::timeGetTime() - m_dwPauseTime;
	}
}

//////////////////////////////////////////////////////////////////////////////
LRESULT CTimer::PauseAll(void){
	if (m_aTimer.empty()) return 0;
	for (set<CTimer*>::iterator it=m_aTimer.begin();it!=m_aTimer.end();it++){
		(*it)->Pause();
	}
	return 0;
}

LRESULT CTimer::RestartAll(void){
	if (m_aTimer.empty()) return 0;
	for (set<CTimer*>::iterator it=m_aTimer.begin();it!=m_aTimer.end();it++){
		(*it)->Restart();
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//	class CTimerEx

// static members..
DWORD	CTimerEx::m_dwTimeGetTime = 0;

DWORD	CTimerEx::Get(void){
	if (m_bPaused>0) return m_dwPauseTime-m_dwOffsetTime;
	return (DWORD)(m_dwTimeGetTime-m_dwOffsetTime);
}

void	CTimerEx::Flush(void){
	m_dwTimeGetTime = CTimeBase::timeGetTime();
}

void	CTimerEx::Set(DWORD dw) {
	if (m_bPaused>0) {
		//	����񂩂ȁ`�H
		m_dwOffsetTime = m_dwPauseTime-dw;
	} else {
		m_dwOffsetTime = (DWORD)(m_dwTimeGetTime - dw);
	}
}

void	CTimerEx::Reset(void) {
	m_dwOffsetTime = m_dwTimeGetTime;
	m_bPaused = 0;
}

void	CTimerEx::Pause(void){
	if (m_bPaused++ == 0) {
	//	Pause������
		m_dwPauseTime = m_dwTimeGetTime;
	}
}

void	CTimerEx::Restart(void){
	if (--m_bPaused == 0) {
	//	Pause����
		m_dwOffsetTime += m_dwTimeGetTime - m_dwPauseTime;
	}
}