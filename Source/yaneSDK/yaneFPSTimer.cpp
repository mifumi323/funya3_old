#include "stdafx.h"

#include "yaneFPSTimer.h"
#include "yaneTimer.h"

CFPSTimer::CFPSTimer(void) {
	SetFPS(60); // �f�B�t�H���g�l�̐ݒ�

	// timeGetTime�̎������グ�邩�H�i�m�s�ł�1ms���ۏ؂���Ă��Ȃ��j
	// �܂��A�ň��A�e���̗L�肻���Ȃ̂́ADrawScene�����Ȃ񂾂���...
}

CFPSTimer::~CFPSTimer() {
	//	�f�X�g���N�^�ł͓��ɏ����͂��Ȃ�
}

void CFPSTimer::SetFPS(DWORD fps){ // FPS�l
	ResetTimer();
	ResetElapseTimeCounter();

	m_dwCallCount = 0;
	m_nUnuseSleep = 0;
	m_nDrawCount  = 0;

	m_dwFPS = fps;
	if (fps==0) {	// non-wait mode
		return ;
	}
	
	// �P�t���[�����Ƃɉ�ms�҂K�v������̂��H�~0x10000[ms]
	m_dwFPSWait = (1000*0x10000)/m_dwFPS;
	
	// �����́AdwFPSWait�̏����ȉ���16�r�b�g�̐��x�ŕێ����邽�߂ɂ���
	m_dwFPSWaitTT = 0; //�@����̎��Ԃ̓[��(cf.DrawFlip)

}

DWORD CFPSTimer::GetFPS(void) {
	return m_dwFPS;
}

void CFPSTimer::ResetTimer(void){
	m_dwLastDraw = CTimeBase::timeGetTime(); // �O��`�掞�Ԃ́A�����Őݒ�
	m_bFrameSkip = false;
	m_dwFrameSkipCounter = 0;
	m_dwFrameSkipCounterN = 0;
}

void CFPSTimer::ResetElapseTimeCounter(void){	 // FPSTime�J�E���^�̃��Z�b�g
	m_dwElapsedTime = 0;
}

DWORD CFPSTimer::GetElapseTimeCounter(void){	 // FPSTime�J�E���^�̎擾
	return m_dwElapsedTime;
}

void CFPSTimer::ResetCallCounter(void){	  // �ďo���J�E���^�̃��Z�b�g
	m_dwCallCount = 0;
}

DWORD CFPSTimer::GetCallCounter(void){	 // �ďo���J�E���^�̎擾
	return m_dwCallCount;
}
// ---------------------------------------------------------------------------

void CFPSTimer::WaitFrame(void){	  // (C)yaneurao 1998-1999
	DWORD t = CTimeBase::timeGetTime(); // ���ݎ���

	//	�X�L�b�v���[�g�J�E���^
	if (m_dwFPS!=0 && ((m_dwCallCount % m_dwFPS) == 0)) {
		m_dwFrameSkipCounter = m_dwFrameSkipCounterN;
		m_dwFrameSkipCounterN = 0;
	}

	m_dwDrawTime[m_nDrawCount & 31] = t;  // Draw�������Ԃ��L�^���邱�Ƃ�FPS���Z�o����菕���ɂ���
	if (++m_nDrawCount == 64) m_nDrawCount = 32;
	// 8�ɖ߂����Ƃɂ���āA0�`15�Ȃ�A�܂�16�t���[���̕`�悪�I����Ă��Ȃ����߁A
	// FPS�̎Z�o���o���Ȃ����Ƃ�m�邱�Ƃ��o����B

	m_dwCallCount++; // ������FPS����Ɏg�����Ƃ��o����B

	// ���Ȃ茵�l�����m�������ǂ����ԑ҂�������͂��B
	if (m_dwFPS == 0) {
		m_dwElapseTime[m_nDrawCount & 31] = 0;
		return ; // Non-wait mode
	}

	m_dwFPSWaitTT = (m_dwFPSWaitTT & 0xffff) + m_dwFPSWait; // ����̑҂����Ԃ��v�Z
	// m_dwFPSWait�́A�҂����Ԃ̏����ȉ����P�U�r�b�g�̐��x�Ŏ����Ă���ƍl����
	// ����ɂ��Adouble�^�������o���K�v���Ȃ��Ȃ�B

	DWORD dwWait = m_dwFPSWaitTT >> 16; // ���ǂ̂Ƃ���A����͉�ms�҂˂�H

	// �P�t���[�����Ԃ��o�߂������B�������ɕ`�悵�Ȃ��Ⴂ�I
	DWORD dwElp = (DWORD)(t - m_dwLastDraw); // �O��`�悩�炢����o�߂��Ƃ�˂�H
	if (dwElp>=dwWait) { // �߂��Ă���I�߂��Ă镪�A250ms�܂ł�����玟��Ɏ����z���ŁI
		DWORD dwDelay = dwElp-dwWait;

		//	250�ȏ�x��Ă�����A�t���[���X�L�b�v���Ȃ��i�������̂��߁j
		//	�����āA�x�ꎞ�Ԃ�0�Ƃ��Ĉ���
		if (dwDelay >= 250) {
			dwDelay = 0;
		}

		//	�Q�t���[���̕`�掞�Ԉȏ�Ȃ�Ύ��t���[�����X�L�b�v����
		m_bFrameSkip =	(dwDelay >= dwWait*3);
		if (m_bFrameSkip) m_dwFrameSkipCounterN++;

		if (dwDelay < 250) { t -= dwDelay; } else { t -= 250; }
		// ����̕`�掞�����U�邱�ƂŁA����̕`��J�n�����𑁂߂�

		m_dwLastDraw = t;
		m_dwElapseTime[m_nDrawCount & 31] = 0;
		return ;
	}

	// �قȁA���Ԃ�ׂ��Ƃ������I

	m_dwElapsedTime += dwElp; // ���ԑ҂��������Ƃ��Čv��
	m_dwElapseTime[m_nDrawCount & 31] = dwElp;

	m_bFrameSkip = false;	//	���̓t���[���X�L�b�v���Ȃ�

/*
	// �܂����Ԃ͂����Ղ肠��̂��H
	// 4ms�ȏ�����K�v������̂Ȃ�΁ASleep����
	// ����dwWait>dwElp�Ȃ̂�dwWait-dwElp>=0�ƍl���ėǂ�
	if (dwWait-dwElp >= 4) {
		if (m_nUnuseSleep) {
			m_nUnuseSleep--;
		} else {
			::Sleep(dwWait-dwElp-3);
		}
	}

	// 95/98/NT�ő��肵���Ƃ���Sleep(1);��1ms�P�ʂŃX���[�v����͉̂\
	// �������A�����n�ˑ��̉\��������̂�Sleep�̐��x��3ms�ȓ��Ɖ���	

	if ((CTimeBase::timeGetTime()-m_dwLastDraw)>=dwWait) {
	// ����ׁ[�I�I�Q�߂����Ƃ��񂯁I�I�i�΁j
		m_nUnuseSleep = 60; // 60�t���[����Sleep�g����͂��...
		// �������A�Ȃ񂿂�[�_�o���ȃv���O�����Ȃ��...
		m_dwLastDraw += dwWait; // �Q�߂����Ă��[�����ǖ�����:p
		return ;
	}

	while ((CTimeBase::timeGetTime()-m_dwLastDraw)<dwWait) ;
	// ���[�v�Ŏ��Ԃ�ׂ��i���܂�D������Ȃ����ǁj
*/
	//	�ˁ@�Ƃ������A���̃X���b�h���ASleep�⃁�b�Z�[�W�������s�Ȃ�Ȃ�
	//	�ꍇ�A���̃X���b�h���狭���I�Ɏ��̃X���b�h�ɐ؂�ւ�����̂�
	//	20ms(���������ȁH)��ł��邩�炵��..���̂ւ���l�������...
	//	��ɃX���[�v�g�������ł������悤�ȋC������...
	::Sleep(dwWait-dwElp);


	// ����ŁA���ԂԂ������I

	m_dwLastDraw += dwWait; // �҂�����ŕ`�悪�����������肷��B�i�[�����������܂Ȃ����߁j
}

bool	CFPSTimer::ToBeSkip(void) const {
	return m_bFrameSkip;
}

DWORD CFPSTimer::GetSkipFrame(void) const {
	return m_dwFrameSkipCounter;
}

DWORD CFPSTimer::GetRealFPS(void) {	//	FPS�̎擾�i����l�j

	if (m_nDrawCount < 16) return 0; // �܂�16�t���[���v�����Ă��Ȃ�
	if (m_nDrawCount < 32) {
		DWORD t = m_dwDrawTime[(m_nDrawCount-1)]	// �O�񎞊�
			-	m_dwDrawTime[(m_nDrawCount-16)];	// 15��O�̎���
		if (t==0) {
			return 0;	//	����s�\
		}	
		return (1000*15+t/2)/t;
		// ���ς���Z�o���Ēl��Ԃ��i�[���͎l�̌ܓ�����j
	}
		DWORD t = m_dwDrawTime[(m_nDrawCount-1) & 31] // �O�񎞊�
			-	m_dwDrawTime[m_nDrawCount & 31];	 // 31��O�̎���
		if (t==0) {
			return 0;	//	����s�\
		}	
		return (1000*31+t/2)/t;
}

DWORD CFPSTimer::GetCPUPower(void) {	 //	 CPU Power�̎擾�i����l�j

	if (m_nDrawCount < 16) return 0; // �܂�16�t���[���v�����Ă��Ȃ�
	DWORD t=0;
	for(int i=0;i<16;i++) 
		t += m_dwElapseTime[i]; // ����16�t���[������FPS��������
	// return 1-t/(1000*16/m_dwFPS)[%] ; // FPS�m���}����Z�o���Ēl��Ԃ�
	return 100-(t*m_dwFPS/160);
}