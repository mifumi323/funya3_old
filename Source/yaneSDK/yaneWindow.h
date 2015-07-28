// yaneWindow.h
//	window base class
//
//	'00/08/04	sohei	CreateSimpleWindow �ǉ�

#ifndef __yaneWindow_h__
#define __yaneWindow_h__

#include "yaneWinHook.h"

class CWindowOption {
public:
	string	caption;	//	�L���v�V����
	string	classname;	//	�N���X��(caption�Ɠ����ł��ǂ�)

	int		size_x;		//	�������̃T�C�Y
	int		size_y;		//	�c�����̃T�C�Y
	LONG	style;		//	�E�B���h�D�X�^�C���̒ǉ��w��	

	CWindowOption(void) { style = WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU; }
};

class CWindow : public CWinHook {
public:
	// �������B�e�E�B���h�E�����݂���̂Ȃ�΍Ō�̃p�����[�^�Ŏw�肷�邱��
	LRESULT		Create(CWindowOption& opt,HWND lpParent=NULL);
	LRESULT		CreateSimpleWindow(CWindowOption& opt,HWND lpParent=NULL);
	HWND		GetHWnd(void)const { return m_hWnd; }	//	HWND��Ԃ�
	LRESULT		SetWindowPos(int x,int y);				//	�E�B���h�D���ړ�
	void		ChangeWindowStyle(void);				//	���݂̃X�N���[�����[�h�p��WindowStyle��ύX����
	LRESULT		Resize(int sx,int sy);					//	���̃��T�C�Y

	//	�؂�ւ������������ꍇ�A���̊֐����Ăяo��������
	//	���ꂼ��̃E�B���h�D�Ɋւ���ChangeWindowStyle���Ăяo������
	//	�������A�t���X�N���[�����̃}���`�E�B���h�D��DirectDraw���g���֌W��A��T�|�[�g
	static	void	ChangeScreen(bool bFullScr) { g_bFullScreen = bFullScr; }

	void		SetSize(int sx,int sy);
	//	���̃��T�C�Y�i�ݒ�݂̂Ŏ��ۂɕύX�͂��Ȃ��j
	//	�Ȃ�����Ȃ��̂��K�v�ɂȂ邩�Ƃ����ƁAResize��ChangeWindowStyle�Ǝ��s�����
	//	�Q��E�B���h�D�T�C�Y�̕ύX���s�Ȃ����ƂɂȂ��āA���̃��[�V�����������č��邩��B
	//	SetSize�@�ˁ@ChangeWindowStyle�Ȃ�Έ��S�B

	//	MouseLayer
	void	UseMouseLayer(bool bUse);	//	�\�t�g�E�F�A�J�[�\���̂��߂ɃJ�[�\��������
	void	ShowCursor(bool bShow);		//	�n�[�h�E�F�A�}�E�X�J�[�\���̕\���^��\��
	bool	IsShowCursor(void);			//	�n�[�h�E�F�A�}�E�X�J�[�\���̕\���^��\�����擾

	//	Property
	DWORD		m_dwFillColor;		//	�w�i�F
//	CWindowOption* GetWindowOption(void) { return &m_opt; }
	volatile bool	IsMinimized(void) { return m_bMinimized; }	//	�ŏ�������Ă��邩�H

	//	���b�Z�[�W���t�b�N���邽�߂̃|�C���^���X�g
	CWinHookList* GetHookList(void) { return &m_HookPtrList; } // ���b�Z�[�W�̃t�b�N���X�g��^����
	void	ClearAllHook(void) { m_HookPtrList.Clear(); }

	CWindow(void);
	virtual ~CWindow();

	// ���j���[�̑��݂��`�F�b�N���A�t���O���X�V���� by ENRA
	void CheckMenu(void) {
		if(m_hWnd!=NULL&&::GetMenu(m_hWnd)!=NULL){
			m_bUseMenu = true;
		}else{
			m_bUseMenu = true;
		}
	}

	//	���̑�
	static void	GetScreenSize(int &x,int &y);			//	���݂̉�ʑS�̂̃T�C�Y�̎擾
	static bool	IsFullScreen(void) { return g_bFullScreen; }	//	�t���X�N���[�����H

protected:
	HWND		m_hWnd;				//	�E�B���h�D�n���h��
	bool		m_bFullScreen;		//	���݂ǂ���̃��[�h�ɍ��킹�đ�������Ă���̂��H
	CWindowOption	m_opt;			//	�E�B���h�D�I�v�V����

	static LRESULT CALLBACK gWndProc(HWND,UINT,WPARAM,LPARAM);
	LRESULT Dispatch(HWND,UINT,WPARAM,LPARAM);	//	windows message dispatcher

	LRESULT		Initializer(void);	//	�N����A��x�����E�B���h�D�N���X��o�^����
	//	�t�b�N���Ă��邷�ׂẴC���X�^���X�ւ̃`�F�C��
	CWinHookList	m_HookPtrList;

	////////////////////////////////////////////////////////////////////////////

	//	�}�E�X�J�[�\����On/Off�́A�E�B���h�D�ɑ΂��鑮���Ȃ̂�
	//	�E�B���h�D�N���X���S���ׂ�
//	void	InnerShowCursor(bool bShow);
	bool	m_bShowCursor;			//	�}�E�X�J�[�\���̕\�����
	bool	m_bUseMouseLayer;		//	�\�t�g�E�F�A�}�E�X�J�[�\�����g�����H

	//	�E�B���h�D�T�C�Y��adjust
	void	InnerAdjustWindow(RECT&,CWindowOption&);

	//	���j���[�t���E�B���h�D���H
	bool	m_bUseMenu;

	//	ChangeWindowStyle��SetSize�̃t���O
	bool	m_bResized;

	//	Windows��{���^�C�v���쐬�����Ƃ��ɁA���b�Z�[�W�n���h����
	//	�t�b�N����̂ŁA���̊֐��|�C���^��ۑ����Ă����K�v������B
	WNDPROC	m_pWndProc;

	//	�ŏ�������Ă��邩�H
	volatile bool	m_bMinimized;

	//	���ڂ̐������ǂ���
	static bool	m_bFirstUserClass;
	bool IsWindowsClassName(string szClassName);
	//	Windows�ŗp�ӂ���Ă���Window�N���X�����ǂ����𒲂ׂ�

/*
	//	���j���[���m���ɑ��݂��邱�Ƃ����O�ɓ`���Ă�����
	//	�������ɐ��m�ȃE�B���h�D�T�C�Y�������Ȃ苁�܂�
	void		UseMenu(bool bUseMenu) { m_bUseMenu = bUseMenu; }
*/	//	�����I�ɔ��肷��悤�ɂ���

	static	bool g_bFullScreen;			//	�t���X�N���[�����[�h�Ȃ̂��H

	// override from CWinHook
	virtual LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

};

#endif