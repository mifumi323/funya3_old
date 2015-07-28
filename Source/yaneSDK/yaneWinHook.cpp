#include "stdafx.h"
#include "yaneWinHook.h"

void CWinHookList::Add(CWinHook* hook)
{
	m_oCriticalSection.Enter();
	{
		m_HookPtrList.push_back(hook);
	}
	m_oCriticalSection.Leave();
}

void CWinHookList::Del(CWinHook* hook){

	// ������Hook�������T���č폜���Ă䂭�i�������肤��j
	m_oCriticalSection.Enter();	//	�폜���ɑ��̃v���Z�X��������Ă���Ǝ��ʂ���
	{
		for(list<CWinHook*>::iterator it=m_HookPtrList.begin();it!=m_HookPtrList.end();) {
			if (*it==hook) {
				it = m_HookPtrList.erase(it);
			} else {
				it ++;
			}
		}
	}
	m_oCriticalSection.Leave();
}

void CWinHookList::Clear(void){

	m_oCriticalSection.Enter();	//	�폜���ɑ��̃v���Z�X��������Ă���Ǝ��ʂ���
	{
		m_HookPtrList.clear();
	}
	m_oCriticalSection.Leave();
}

//	message dispatcher
//	  CWindow��WndProc����Ăяo�����̂ŁA����CriticalSection�́A�e�E�B���h�D�ɂ�����݂��邱�ƂɂȂ�B
//	  �܂肱��CriticalSection�́A���b�Z�[�W���[�v�ƁA����ɑΉ����郁�C���X���b�h�Ԃł̑��ݔr���̂��߂̂��̂ł���B
LRESULT CWinHookList::Dispatch(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc){

	m_oCriticalSection.Enter();
	{
		list<CWinHook*>::iterator it=m_HookPtrList.begin();
		while (it!=m_HookPtrList.end()) {
			LRESULT l;
			l = (*it)->WndProc(hWnd,uMsg,wParam,lParam);
			if (l) {
				m_oCriticalSection.Leave();
				return l; // �Ԃ��l�Ƃ��ĂO�ȊO�����Ȃ�΃��b�Z�[�W�͏������ꂽ�ƌ��Ȃ�
			}
			it ++ ;
		}
	}
	m_oCriticalSection.Leave();

	if (pWndProc == NULL) {
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	} else {
		//	hook�����E�B���h�D�֐����Ăяo��
		return ::CallWindowProc(pWndProc,hWnd,uMsg,wParam,lParam);
	}
}