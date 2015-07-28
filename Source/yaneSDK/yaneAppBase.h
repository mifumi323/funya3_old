//	yaneApp.h :
//		application thread class
//			programmed by yaneurao	'00/02/25

#ifndef __yaneAppBase_h__
#define __yaneAppBase_h__

#include "yaneWinHook.h"
#include "yaneWindow.h"
#include "yaneThread.h"

class CAppBase : public CWinHook,public CThread {
public:

	virtual LRESULT Run(void);			//	���̍쐬�i�ƃ��b�Z�[�W���[�v for MainApp

	HWND	GetHWnd(void) const { return m_oWindow.GetHWnd(); } //	�E�B���h�D�n���h���擾
	CWindow*GetMyWindow(void)	{ return &m_oWindow; }		//	�E�B���h�D�N���X�擾

	bool	IsMainApp(void) const { return this==m_lpMainApp; }
	bool	IsMessage(void) const { return m_bMessage; }			//	���b�Z�[�W���[�v�X���b�h�͐����Ă��邩

	//	����g�����ȁH
	static	CAppBase* GetMainApp(void)	 { return m_lpMainApp; }
	static	HWND	GetMainWnd(void)	 { return GetMainApp()->GetHWnd(); }

	//	�X���b�h�̐������`�F�b�N overriden from CThread
	bool	IsThreadValid(void);

	//	�X���b�h��Invalidate�B
	virtual void InvalidateThread(void);	//	overriden from CThread

	//	�E�B���h�D�ŏ������ɑҋ@���邩
	volatile bool*	GetWaitIfMinimized(void) {	return &m_bWaitIfMinimized; }

	//	OnPreClose�����������ɋ����I��WM_CLOSE�����s����
	virtual void	Close(void);

	//	Idle���[�h(default:false)�ɂ���̂�
	bool* GetIdle(void) { return &m_bIdle;}

	CAppBase(void);
	virtual ~CAppBase();

protected:
	//////////////////////////////////////////////////////////////////////////
	//	�p������Ƃ������Ƃ��邩���i�΁j�֐��̈ꗗ

	//	���ꂪ���s�X���b�h�Ȃ̂Ōp�����āA������p�ӂ��ĂˁI
	//	�E�B���h�D�쐬��(OnCreate()�̂���)�ɌĂяo�����́B
	virtual void MainThread(void) { while (IsThreadValid()) ThreadSleep(20); }

	//	���̑����K�X�I�[�o�[���C�h���Ă˂�
	virtual LRESULT OnInit(void)	{ return 0; }		//	��������
	virtual LRESULT OnPreCreate(CWindowOption& opt);	//	�E�B���h�D�쐬���O
	virtual LRESULT OnCreate(void)	{ return 0; }		//	�E�B���h�D�쐬����
	virtual LRESULT OnDestroy(void) { return 0; }		//	�I�����O

	virtual LRESULT OnPreClose(void) { return 0; }
	//	WM_CLOSE�����O�ɌĂяo����A������0��Ԃ���WM_CLOSE�͎��s����Ȃ�
	//	���̏󋵉��ɂ����ăE�B���h�D��CLOSE�����邽�߂ɂ́AClose���Ăяo������

	//	brief Window Message to override
	virtual void OnPaint(void)		{ }

	//////////////////////////////////////////////////////////////////////////
protected:
	CWindow m_oWindow;			//	�ۗL���Ă���window
	bool	m_bMessage;			//	���b�Z�[�W���[�v�X���b�h�͐����Ă��邩

	static CAppBase* m_lpMainApp;	//	���C���E�B���h�D�i����̏I�����v���O�����I���Ƃ݂Ȃ��j

	virtual void	ThreadProc(void);					//	override from CThread
	virtual LRESULT WndProc(HWND,UINT,WPARAM,LPARAM);	//	override in CWinHook
	void	InnerStopThread(void);

	volatile bool	m_bWaitIfMinimized;		//	�E�B���h�D�ŏ������ɑҋ@���邩

	bool	m_bClose;			//	OnPreClose���Ă΂���WM_CLOSE���������邩�H
	bool	m_bIdle;			//	Idle���[�h

private:
	static	DWORD WINAPI RunPrepare(LPVOID lpVoid); //	Run����̃W�����v��
	LRESULT RunThread(void);						//	�E�B���h�D�̍쐬��WorkThread�̍쐬��MessageLoop
	LRESULT MessageLoop(void);						//	MessageLoop
	volatile LONG	m_nThreadStatus;				//	�X���b�h�̃X�e�[�^�X
};

#endif