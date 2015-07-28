// yaneWinHook.h
//	WindowMessageHooker
//	  Window���b�Z�[�W���t�b�N�����邱�Ƃ�Window�̃T�u�N���X�����A�V�X�g����
//	  (c) yaneurao '99/06/23
//
//		programmed by yaneurao(M.Isozaki)	'99/06/21
//		reprogrammed by yaneurao			'00/02/25

#ifndef __yaneWinHook_h__
#define __yaneWinHook_h__

#include "yaneCriticalSection.h"

class CWinHook {
public:
	virtual	LRESULT	WndProc(HWND,UINT,WPARAM,LPARAM) = 0;
};

class CWinHookList {
public:
	void	Add(CWinHook*);				//	�������g���t�b�N�ɉ�����
	void	Del(CWinHook*);				//	�������g���t�b�N����O��
	void	Clear(void);				//	���ׂĂ��N���A����

	//	���b�Z�[�W��Dispatcher
	LRESULT Dispatch(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc=NULL);

protected:
	list<CWinHook*>		m_HookPtrList;	//	�t�b�N���X�g
	CCriticalSection	m_oCriticalSection;
};

#endif