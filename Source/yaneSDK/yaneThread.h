//	yaneThread.h :
//		programmed by yaneurao	'00/02/26
//		�����I�u�W�F�N�g�̓��� '01/11/19

#ifndef __yaneThread_h__
#define __yaneThread_h__

class CThread {
public:
	virtual void ThreadProc() = 0;	//	������I�[�o�[���C�h���ĂˁI

	virtual LRESULT CreateThread();
	virtual LRESULT StopThread();
	virtual LRESULT JumpToThread();	//	�X���b�h����炸�ɃW�����v
	virtual bool IsThreadExecute()const { return m_bThreadExecute; }
	virtual bool IsThreadValid()const	{ return m_bThreadValid;   }
	virtual void InvalidateThread();

	//	Thread�������Ă��邱�Ƃ����������I�u�W�F�N�g
	virtual	LRESULT ThreadSleep(int nTime);
	//	nTime [ms]�҂B�҂��Ă���Œ��ɁA�����I�u�W�F�N�g���V�O�i�����
	//	�i�X���b�h���I�������Ȃ����A�Ƃ�����ԁj�ɂȂ�΁A��0���Ԃ�B

	CThread();
	virtual ~CThread();

protected:
	volatile bool m_bThreadExecute;	//	Thread�͎��s���Ȃ̂��H
	volatile bool m_bThreadValid;	//	Thread���~���������Ƃ���false�ɂ���
//	static DWORD WINAPI ThreadCallBack(LPVOID lpVoid);
	static void ThreadCallBack(LPVOID lpVoid);
	volatile DWORD	m_dwThreadID;		//	�X���b�h�n���h��
	HANDLE	m_hEventObject;				//	�����I�u�W�F�N�g
	virtual void InnerCreateEvent();	//	���̍쐬
	virtual void InnerDeleteEvent();	//	���̉��
};

#endif