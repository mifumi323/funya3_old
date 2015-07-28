//	�A�v���P�[�V�����C�j�V�����C�U
//		yaneuraoGameSDK 2nd�̂��߂̃C�j�V�����C�U�B
//		�C�ɂ���Ȃ���Ύ��R�ɃJ�X�^�}�C�Y���Ďg���Ă��������ȁO�O

#ifndef __yaneAppInitializer_h__
#define __yaneAppInitializer_h__

#include "yaneWinHook.h"

class CAppInitializer {
public:
	//	WinMain���炻�̃p�����[�^�����̂܂ܓn���ĂˁB
	static void Init(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow);

	static HINSTANCE GetInstance(void)			{ return m_hInstance; }
	static HINSTANCE GetPrevInstance(void)		{ return m_hPrevInstance; }
	static LPSTR	 GetCmdLine(void)			{ return m_lpCmdLine; }
	static int		 GetCmdShow(void)			{ return m_nCmdShow; }

	//	�����̑�����A�v���X���b�h�ۗ̕L���Ă���E�B���h�D�Ɋ֘A�t����B
	//	�i�E�B���h�D���b�Z�[�W���R�[���o�b�N�����悤�ɂȂ�j
	static	void	Hook(CWinHook*p);
	static	void	Unhook(CWinHook*p);

	//	�����̑�����A�v���X���b�h�ۗ̕L����E�B���h�D�n���h����Ԃ�
	static	HWND	GetHWnd(void);

	//	�t���X�N���[�����[�h���ǂ�����Ԃ�
	static	bool	IsFullScreen(void);

protected:
	//	instance parameter...
	static	HINSTANCE	m_hInstance;
	static	HINSTANCE	m_hPrevInstance;
	static	LPSTR		m_lpCmdLine;
	static	int			m_nCmdShow;
};

#endif