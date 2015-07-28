//	yaneIntervalTimer.cpp
#include "stdafx.h"
#include "yaneAppIntervalTimer.h"

chain<CAppIntervalTimer> CAppIntervalTimer::m_alpTimerList;	//	�R�[���o�b�N���ׂ����X�g

CAppIntervalTimer::CAppIntervalTimer(void){
	m_bStop	 = true;
}

CAppIntervalTimer::~CAppIntervalTimer(){
	Stop();
	m_alpTimerList.erase(this);	//	���O�ō폜���Ȃ��Ă͂Ȃ�Ȃ�
}

bool	CAppIntervalTimer::IsTimer(void) const {
	return !m_bStop;
}

LRESULT CAppIntervalTimer::Start(void){
	//	set�̎d�l�ɂ��A���ɑ��݂���̂Ȃ��insert����Ȃ�
	m_alpTimerList.insert(this);
	m_bStop = false;
	return 0;
}

LRESULT CAppIntervalTimer::Stop(void){
//	m_alpTimerList.erase(this);
	//	��������erase����ƁA�R�[���o�b�N�̒��ō���^^;
	m_bStop = true;
	return 0;
}

void	CAppIntervalTimer::TimerCallBackAll(void){
	//	�����Ă��邷�ׂẴ^�C�}�ɃR�[���o�b�N��������
	chain<CAppIntervalTimer>::iterator it = m_alpTimerList.begin();
	while (it!=m_alpTimerList.end()){
		if (!(*it)->IsTimer()) {
			//	�K�[�x�W���[�����˂�O�O�G
			it = m_alpTimerList.erase(it);
		} else {
			(*it)->TimerProc();
			it++;
		}
	}
}