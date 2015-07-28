#include "stdafx.h"
#include "yaneThread.h"
#include <process.h> 

//////////////////////////////////////////////////////////////////////////////

CThread::CThread(){
	m_dwThreadID	 = -1;
	//	-1�̓G���[�l�B(�X���b�h�n���h���Ƃ��Ă͂��肦�Ȃ��l)
	//	�i_beginthreadex�͗��_��͂O���Ԃ��Ă��邱�Ƃ�����j

	m_bThreadExecute = false;
	m_bThreadValid	 = false;
	m_hEventObject	= NULL;
}

CThread::~CThread(){
	StopThread();
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CThread::CreateThread() {
	StopThread();
//	return !::CreateThread(NULL,0,ThreadCallBack,this,NULL,&m_dwThreadID);
	m_dwThreadID = ::_beginthread(ThreadCallBack,0,(void*)this);
	/*
					�������ŃR���p�C���G���[���o��Ƃ�����A
				�����N���郉�C�u�������V���O���X���b�h�ɂȂ��Ă��邩��ł��B
		�v���W�F�N�g�̐ݒ�ˁuC/C++�v�ˁu�R�[�h�����v�ŁA
		�u�g�p���郉���^�C�����C�u�����v�́A�}���`�X���b�h�p�̂��̂��g�p���Ă��������B
	*/

	if (m_dwThreadID==-1) {
		return 1;
	}
	return 0;
		//	��_beginthreadex�́A�G���[���A-1���Ԃ��Ă���
	/*
		CreateThread�ł͂Ȃ�_beginthread�ŃX���b�h�𐶐����Ȃ��ƁA
		C����̃����^�C�����Ăяo���Ă���ꍇ�A���K�͂ȃ��������[�N����������B(80�o�C�g���x)
		���ƁA�R���p�C������Ƃ��́A�}���`�X���b�h�p�̃��C�u�����������N����悤�ɁB
	*/
}

LRESULT CThread::StopThread() {
	InvalidateThread();			//	�~�܂��ė~�����˂�I

/*
	while (m_bThreadExecute)	//	�܂����ȁA�܂�����...
		::Sleep(10);
	�����̕��@�ŁA�X���b�h�̏I����҂ƁA�X���b�h�̌㏈�����I�����Ă��Ȃ��̂�
	���̃t���O���|�ꂽ�u�ԁA�X���b�h���I���������̂Ƃ��āA���C���X���b�h��
	�I�����Ă��܂��A���̃X���b�h���c�[�Ƃ��Ďc��\��������B
*/
	if (m_dwThreadID==-1) return 0;
	//	�X���b�h�����Ăւ�ŁH

	//	����A���܂�ǂ��Ȃ��Ƃ͎v�����ǁA�X���b�h�n���h�����d������قǒ�����
	//	�i�����炭�A���N�j�d���������ςȂ��Ŏg�p
	//	�����邱�Ƃ͖����Ǝv����̂ŁA�l���Ȃ����Ƃɂ���
	DWORD dwResult = ::WaitForSingleObject((HANDLE)m_dwThreadID,INFINITE);
	m_dwThreadID = -1;
	if (dwResult != WAIT_OBJECT_0) {
		//	�����ƁA���̃X���b�h�n���h���̏��L�������ł�
		//	�j������Ă����̂��Ǝv��..
		return 1;
	}
	//	�I�������̂ŁD�D
	return 0;
}

LRESULT CThread::JumpToThread() {
	InnerCreateEvent();
	ThreadProc();				//	��������s����̂���
	InnerDeleteEvent();
	return 0;
}

void CThread::ThreadCallBack(LPVOID lpVoid){
	CThread* pThread = reinterpret_cast<CThread*>(lpVoid);
	pThread->InnerCreateEvent();
	pThread->ThreadProc();
	pThread->InnerDeleteEvent();	// �����I�u�W�F�N�g����̂��Ƃ����ƁO�O�G
}

void	CThread::InvalidateThread(){
	m_bThreadValid = false;
	//	�����I�u�W�F�N�g���V�O�i����Ԃɂ��Ă��

	::SetEvent(m_hEventObject);
}

///////////////////////////////////////////////////////////////////////////
//	�����I�u�W�F�N�g�̓��� '01/11/19


void	CThread::InnerCreateEvent(){
	InnerDeleteEvent();	//	�O�̂����̂��˂΁I
	m_hEventObject = ::CreateEvent(
		NULL,	//	security descriptor
		FALSE,	//	auto reset
		FALSE,	//	not signal state
		NULL	//	event object name
	);
	//	NULL�Ȃ�ΐ����G���[�Ȃ̂����D�D
	m_bThreadExecute = true;
	m_bThreadValid	 = true;
}

void	CThread::InnerDeleteEvent(){
	if (m_hEventObject!=NULL){
		::CloseHandle(m_hEventObject);
		m_hEventObject = NULL;
	}
	m_bThreadExecute = false;
}

LRESULT CThread::ThreadSleep(int nTime){
//	nTime [ms]�҂B�҂��Ă���Œ��ɁA�����I�u�W�F�N�g���V�O�i�����
//	�i�X���b�h���I�������Ȃ����A�Ƃ�����ԁj�ɂȂ�΁A��0���Ԃ�B
	if (m_hEventObject == NULL) return -1;

	DWORD dwResult = ::WaitForSingleObject(m_hEventObject,nTime);
	if (dwResult == WAIT_TIMEOUT) {
		//	�I�b�P�[�I�@�܂��܂�������i�΁j
		return 0;
	}

	return 1;
}