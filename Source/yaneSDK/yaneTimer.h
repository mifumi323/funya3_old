// yaneTimer.h :
//
//	CTimer			:	�o�ߎ��Ԃ��J�E���g����^�C�}
//
//		programmed by yaneurao(M.Isozaki) '99/07/25
//		modified by yaneurao '00/02/28-'00/03/13

#ifndef __yaneTime_h__
#define __yaneTime_h__

//	�����b��timeGetTime wrapper
class CTimeBase {
public:
	static int		timeBeginPeriodMin(void);	//	�^�C�}���ŏ��ݒ�ɂ���i�Ԃ��l interval[ms]�j
	static void		timeEndPeriodMin(void);		//	�^�C�}�ŏ��ݒ����
	static DWORD	timeGetTime(void) {
		if (m_bUseTGT) {
//			::timeGetSystemTime(&m_mmtime,sizeof(MMTIME));
//			return m_mmtime.u.ms;
			return ::timeGetTime();
		} else {
			return ::GetTickCount();
		}
	}
	///////////////////////////////////////////////////////////////////////
	CTimeBase(void);	// Initilizer
protected:
	static bool		m_bUseTGT;			//	timeGetTime���g�p����̂��H
	static int		m_nRef;				//	timeBeginPeriodMin�`timeEndPeriodMin�̎Q�ƃJ�E���g
	static TIMECAPS	m_dwTimeCaps;		//	�^�C�}�[���\
//	static MMTIME	m_mmtime;			//	�������̂���timeGetSystemTime���g�p
//		���������Ǝv���Ă���A�����Ȃ�����
};

//////////////////////////////////////////////////////////////////////////////

class CTimer {
public:
	void	Reset(void);
	DWORD	Get(void);					//	�����̎擾
	void	Set(DWORD);					//	�����̐ݒ�
	void	Pause(void);				//	Pause�@�\
	void	Restart(void);				//	Pause����
	static LRESULT	PauseAll(void);		//	�S�C���X�^���X��Pause
	static LRESULT	RestartAll(void);	//	�S�C���X�^���X��Pause����

	/////////////////////////////////////////////////////////////////
public:
	CTimer(void);
	virtual ~CTimer();
protected:
	static set<CTimer*>	m_aTimer;			//	�S�C���X�^���X�̃`�F�[��
	DWORD	m_dwOffsetTime;					//	�I�t�Z�b�g�l
	DWORD	m_dwPauseTime;					//	Pause�������Ƃ���Time
	int		m_bPaused;						//	pause�����H
};

//////////////////////////////////////////////////////////////////////////////

class CTimerEx : public CTimer {
public:
	void	Reset(void);
	DWORD	Get(void);					//	�����̎擾
	void	Set(DWORD);					//	�����̐ݒ�
	void	Pause(void);				//	Pause�@�\
	void	Restart(void);				//	Pause����

	static	void Flush(void);			//	�����̃t���b�V��

protected:
	static DWORD m_dwTimeGetTime;			//	�O��Flush��������
};

#endif