//		  yaneMutex Class �̎���
//---------------------------------------------------------------------
// ���܂�������Ȃ����A�Ђ���Ƃ���A�v����d�N���h�~�ȊO�ɂ͎g���Ȃ�����
#include "stdafx.h"
#include "yaneMutex.h"

CMutex::CMutex(void){
	m_hMutex = NULL;
}

CMutex::~CMutex(){
	Release();
}

LRESULT CMutex::Open(string szMutexName){
	// ����v���Z�X�̃X���b�h�ԂłȂ�CriticalSection��p����ׂ��I
	Release();

	// ���̖��O�œo�^
	m_hMutex = ::CreateMutex(NULL,true,szMutexName.c_str());
	if (GetLastError()==ERROR_ALREADY_EXISTS) return 1;
	//	�˃G���[�ł͂Ȃ��A�����ł���Ƃ����Ӗ���1��Ԃ��Ă���
	//	���̏ꍇ���AMutex��Open�ɂ͐������Ă���ƍl���邱�Ƃ��o����B

	// �����̂��ߏ��L�����l���ł��Ă��Ȃ�
	return 0; // ����ȊO�̃G���[���đz��ł��Ȃ����A������łȂ��H
}

void CMutex::Wait(void){
	::WaitForSingleObject(m_hMutex,INFINITE);	// ���L����������܂ő҂�
}

void CMutex::Release(void){
	if (m_hMutex!=NULL) {
		::ReleaseMutex(m_hMutex);	//	���L���̉��
		::CloseHandle(m_hMutex);	//	��������Mutex�̔j��
		//	���̂������Ă���ƁA�����j��ł���Ƃ�����Ȃ����D�D�H

/*
	MSDN���C�u����2001�N4�������[�X���F
�n���h�������ɂ́ACloseHandle �֐����g���܂��B�v���Z�X���I������ۂɁA�V�X�e���͂��̃v���Z�X�����L���Ă����n���h���������I�ɕ��܂��B�~���[�e�b�N�X�I�u�W�F�N�g�ɑ΂��� 1 �܂��͕����̃n���h�����J���Ă���ꍇ�A�Ō�̃n���h�����������_�ŁA���̃~���[�e�b�N�X�I�u�W�F�N�g�͔j������܂��B
*/
		//	�u1 �܂��͕����̃n���h�����J���Ă���v�́u�J���Ă���v�́A
		//	CreateMutex�̂��Ƃ��Ӗ�����̂��Ǝv�����A
		//	�܂�A����Ɋւ��ẮA
		//	�Q�ƃJ�E���g�̂悤�ȏ��������Ă���ƍl���邱�Ƃ��o����̂��낤�D�D

		m_hMutex = NULL;
	}
}

void	CMutexSection::Enter(string szMutexName){
	//	MutexSection�ɓ���
	LRESULT lr = m_vMutex.Open(szMutexName);
	if (lr==0) return ; // �l�������I
	if (lr==1) {
		m_vMutex.Wait();	//	���L�����l������܂ő҂�
		return ;
	}
	return ;
}

void	CMutexSection::Leave(){
	//	MutexSection����o��
	m_vMutex.Release();
}