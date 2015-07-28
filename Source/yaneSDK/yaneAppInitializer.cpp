#include "stdafx.h"

#include "yaneAppInitializer.h"
#include "yaneAppManager.h"
#include "yaneAppBase.h"
#include "yaneWindow.h"
#include "yaneFile.h"

HINSTANCE	CAppInitializer::m_hInstance;
HINSTANCE	CAppInitializer::m_hPrevInstance;
LPSTR		CAppInitializer::m_lpCmdLine;
int			CAppInitializer::m_nCmdShow;

//	WinMain���炻�̃p�����[�^�����̂܂ܓn���ĂˁB
void	 CAppInitializer::Init(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){

	//	�p�����[�^�����̂܂ܕۑ�:p
	m_hInstance		=	hInstance;
	m_hPrevInstance	=	hPrevInstance;
	m_lpCmdLine		=	lpCmdLine;
	m_nCmdShow		=	nCmdShow;

	//	�J�����g�f�B���N�g���̐ݒ�
	CFile::SetCurrentDir();
}

//////////////////////////////////////////////////////////////////////////////
//	���@�V�X�e�����̂P:p
void	CAppInitializer::Hook(CWinHook*p){
	CAppManager::GetMyApp()->GetMyWindow()->GetHookList()->Add(p);
}

void	CAppInitializer::Unhook(CWinHook*p){
	CAppManager::GetMyApp()->GetMyWindow()->GetHookList()->Del(p);
}

//	���@�V�X�e�����̂Q:p
HWND	CAppInitializer::GetHWnd(void) {
	CAppBase* lp = CAppManager::GetMyApp();
	if (lp==NULL) return NULL;
	return lp->GetHWnd();
}

//	���@�V�X�e�����̂R:p
bool	CAppInitializer::IsFullScreen(void){
	return CWindow::IsFullScreen();
}