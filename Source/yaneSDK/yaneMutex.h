// yaneMutex.h :
//
//	programmed by M.Isozaki '99/06/19
//	����n���h�����擾����̂Œx���ł��:p

#ifndef __yaneMutex_h__
#define __yaneMutex_h__

class CMutex {
public:
	LRESULT Open(string szMutexName);

	// Result
	//	0 : ���ɓ����̂��̂͑��݂��Ȃ������L���l�������I
	//	1 : ���ɓ����̂��̂����݂���B(error)
	
	void Wait(void);	// ���L�����l���ł���܂ő҂�

	void Release(void);	//	Mutex���������

	CMutex(void);
	virtual ~CMutex();
protected:
	HANDLE	m_hMutex; // Mutex �n���h�� 
};

//	Mutex�ɂ��Lock�`unlock
//		CCriticalSection�Ɠ������o�Ŏg����
class CMutexSection {
public:
	void	Enter(string szMutexName);	//	MutexSection�ɓ���
	void	Leave();					//	MutexSection����o��

	//	�f�X�g���N�^�ł�CMutex���������̂ŁA���̂Ƃ���
	//	�����I��Mutex�͉�������B

protected:
	CMutex m_vMutex;
};

#endif