// yaneTimer.h :
//
//	CIntervalTimer	:	�}���`���f�B�A�^�C�}
//						�ƒʏ�̃C���^�[�o���^�C�}�[�̓����N���X
//
//		programmed by yaneurao(M.Isozaki) '99/07/25
//		modified by yaneurao '00/02/28-'00/03/13

#ifndef __yaneIntervalTime_h__
#define __yaneIntervalTime_h__

// �}���`���f�B�A�^�C�}�[
class CIntervalTimer {
public:

	CIntervalTimer(void);
	virtual ~CIntervalTimer();

	//	�^�C�}���J�n����
	LRESULT Start(DWORD dwInterval,bool bMultiMediaTimer = true);

	//	������́A�ꔭ�^�C�}�����x���Ăяo��
	LRESULT	StartM(DWORD dwInterval);

	LRESULT Stop(void);					//	�^�C�}���~�߂�
	bool	IsTimer(void) const;		//	�^�C�}������Ă���̂�
	virtual void TimerProc(void) { }	//	�R�[���o�b�N�����֐�

protected:
	UINT		m_hTimer;
	UINT		m_uTimer;
	DWORD		m_dwIntervalMin;	//	�^�C�}call back�̍ŏ��C���^�[�o��
	bool		m_bStop;			//	StartM�ŉ񂵂Ă���^�C�}�̐���p

	DWORD		m_dwDelay;			//	StartM�Ŏg��
	DWORD		m_dwInterval;

private:
	//	TimerProc�ւ�Jump��@�i�}���`���f�B�A�^�C�}�̂Ƃ��j
	static void CALLBACK StaticTimeProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2);
	//	TimerProc�ւ�Jump��@�i�ʏ�̃C���^�[�o���^�C�}�̂Ƃ��j
	static void CALLBACK StaticTimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime);
	//	TimerProc�ւ�Jump��@�i�}���`���f�B�A�^�C�}�Ń����V���b�g�^�C�}�̂Ƃ��j
	static void CALLBACK StaticTimeProcM(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2);
};

#endif