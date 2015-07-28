// yaneAppIntervalTimer.h :
//
//	CAppBase�Ɋ񐶂���^�C�}�O�O�G

#ifndef __yaneAppIntervalTime_h__
#define __yaneAppIntervalTime_h__

class CAppIntervalTimer {
public:

	CAppIntervalTimer(void);
	virtual ~CAppIntervalTimer();

	//	�^�C�}���J�n����
	LRESULT Start(void);
	LRESULT Stop(void);						//	�^�C�}���~�߂�
	bool	IsTimer(void) const;			//	�^�C�}������Ă���̂�
	virtual void TimerProc(void) { }		//	�R�[���o�b�N�����֐�

	static void	TimerCallBackAll(void);			//	�����Ă��邷�ׂẴ^�C�}�ɃR�[���o�b�N��������

protected:
	bool	m_bStop;							//	�^�C�}�~�܂��Ă�H

	static	chain<CAppIntervalTimer> m_alpTimerList;	//	�R�[���o�b�N���ׂ����X�g
};

#endif