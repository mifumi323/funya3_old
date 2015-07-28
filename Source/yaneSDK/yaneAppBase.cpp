#include "stdafx.h"
#include "yaneAppBase.h"
#include "yaneAppManager.h"
#include "yaneAppInitializer.h"
#include "yaneAppIntervalTimer.h"

CAppBase*	CAppBase::m_lpMainApp=NULL;

//////////////////////////////////////////////////////////////////////////////

CAppBase::CAppBase(void) {
	m_bIdle		=	false;
	m_bMessage	=	false;
	m_bWaitIfMinimized = false;
	m_bClose	=	false;	//	OnPreClose�𖳎�����Close�����ԂȂ̂��H

	CAppManager::Inc();	//	�Q�ƃJ�E���g�̃C���N�������g
}

CAppBase::~CAppBase(void) {
/*
	//	���C���X���b�h�́A�X���b�h�ł͖����̂��O�O�G
	StopThread();
	//	���b�Z�[�W���[�v�ƃ��C���X���b�h�̏I����҂�
	while (IsMessage()) {
		::Sleep(100);
	}
*/

	StopThread();
	CAppManager::Dec();	//	�Q�ƃJ�E���g�̃f�N�������g
	//	���̃N���X�́ACThread�h���N���X�Ȃ̂ŁA
	//	��Dec��StopThread����ɂȂ��Ă��܂��B
	//	����āA��s���āAStopThread���Ăяo�����Ƃɂ���āA
	//	�X���b�h�̒�~���ۏ؂����B
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CAppBase::OnPreCreate(CWindowOption& opt){

	opt.caption		= "���Ղ肿���";
	opt.classname	= "YANEAPPLICATION";
	opt.size_x		= 640;
	opt.size_y		= 480;
//	opt.style		= WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION;
	opt.style		= WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CAppBase::Run(void){
	//	���ꂪ���C���X�^���X�Ȃ�΁A�����e�E�B���h�D�Ƃ݂Ȃ�
	if (m_lpMainApp==NULL) m_lpMainApp = this;

	if (IsMainApp()) {
		return JumpToThread();
	}
	//	���C���E�B���h�D�ȊO�Ȃ�΁A�����p�ɃX���b�h����遫

	m_nThreadStatus = -1;
	if (CreateThread()) return 1;

	//	�E�B���h�D�̊����܂ő҂�
	while (true){
		if (m_bMessage || m_nThreadStatus>=0) break;
		::Sleep(100);
	}
	return 0;
}

//	���ꂪ�쐬���ꂽ���C���X���b�h
void CAppBase::ThreadProc(void){		//	override from CThread
	//	�E�B���h�D�̍쐬��WorkThread�̍쐬��MessageLoop
	if (OnInit()) return ;
	CWindowOption opt;
	if (OnPreCreate(opt)) return ;			//	�E�B���h�D�������O�ɌĂяo�����
	if (m_oWindow.Create(opt)) return ;		//	�E�B���h�D�̍쐬
	if (OnCreate()) return ;				//	�E�B���h�D������Ă���Ăяo�����
	CAppManager::Add(this);					//	����CAppBase�̓o�^
	CAppInitializer::Hook(this);			//	���b�Z�[�W�t�b�N�J�n
	m_bMessage = true;						//	����ƃE�B���h�D�͊�������
	MainThread();							//	���[�U�[���ŗp�ӂ��ꂽ�A���C���֐�
	m_bMessage = false;						//	�E�B���h�D�͔j�󂳂��̂�...
	OnDestroy();							//	�I�����O
	//	Thread��app�𔻕ʂ��Ă���̂�Hook����Thread��Del���Ȃ��Ă͂Ȃ�Ȃ�
	//	::SendMessage(GetHWnd(),WM_CLOSE,0,0);	//	���b�Z�[�W�X���b�h���~������
	::SendMessage(GetHWnd(),WM_DESTROY,0,0);	//	���b�Z�[�W�X���b�h���~������
	//	WM_Destory���������Ȃ��Ă͂Ȃ�Ȃ��̂ł����Ńt�b�N����
	CAppInitializer::Unhook(this);			//	���b�Z�[�W�t�b�N�̏I��

	/*
		������CAppManager����؂藣���ƁAStopThread�����O�ɁA
		�I�����肪�s�Ȃ��A�X���b�h���c������\��������B
		����āACThread�̑S�ẴX���b�h���I�������̂��m�F���Ă����~���ׂ�
	*/

	CAppManager::Del(this);					//	����CAppBase�̍폜
	InnerStopThread();						//	�X���b�h���~
}

bool CAppBase::IsThreadValid(void)	{
	//	���̃`�F�b�N�̂Ƃ��ɃX���b�h�̐��������`�F�b�N����
	MSG msg;
	LRESULT lr;
	if ( !m_bIdle ){
		while (::PeekMessage(&msg, NULL /* GetHWnd() */,0,0,PM_REMOVE)) {
			//	�˂����AGetHWnd�ɂ��Ȃ��ƃ}���`�E�B���h�D�ɂ����Ƃ��A�}�Y�C�̂����A
			//�@�ǂ����ANULL�E�B���h�D�ɂ������ł��Ȃ����b�Z�[�W������悤��...

			//	���b�Z�[�W�����݂�����菈�����Â���
			::TranslateMessage(&msg); 
			::DispatchMessage(&msg);
		}
	}else{
		// Idle���[�h�ǉ�
		while( (lr = ::GetMessage(&msg, NULL ,0,0)) ){
			//	�˂����AGetHWnd�ɂ��Ȃ��ƃ}���`�E�B���h�D�ɂ����Ƃ��A�}�Y�C�̂����A
			//�@�ǂ����ANULL�E�B���h�D�ɂ������ł��Ȃ����b�Z�[�W������悤��...
			if (lr==-1) break;
			//	�G���[�R�[�h���Ԃ��Ă��Ă���ꍇ�A�����
			//	dispatch���Ă͂����Ȃ��B
			//	�������A���̏ꍇ�A�A�v�����I��������ׂ����낤���H

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			break;
		}
	}
	CAppIntervalTimer::TimerCallBackAll();	//	�t�b�N����Ă���^�C�}�ɃR�[���o�b�N��������
	if (m_bWaitIfMinimized) {
		//	WM_QUIT���ŏ��������������̂�҂�
		while (GetMyWindow()->IsMinimized() && (lr = GetMessage(&msg,NULL,0,0))) {
			//	�˂����AGetHWnd�ɂ��Ȃ��ƃ}���`�E�B���h�D�ɂ����Ƃ��A�}�Y�C�̂����A
			//�@�ǂ����ANULL�E�B���h�D�ɂ������ł��Ȃ����b�Z�[�W������悤��...
			if (lr==-1) break;
			//	�G���[�R�[�h���Ԃ��Ă��Ă���ꍇ�A�����
			//	dispatch���Ă͂����Ȃ��B
			//	�������A���̏ꍇ�A�A�v�����I��������ׂ����낤���H

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	return (m_bThreadValid);
}

void	CAppBase::InnerStopThread(void){
	if (IsMainApp()) {
		CAppManager::StopAllThread();			//	�S�X���b�h���~������
		m_bThreadValid = false;					//	�V���O���X���b�h�Ȃ̂ŁA�����ˁI
	} else {
		//	�V���O���X���b�h�Ȃ�΂���͌Ăяo���܂ł��Ȃ��A��~���Ă���͂�����
//		StopThread();							//	���C���X���b�h�̏I���ҋ@����
		m_bThreadValid = false;					//	�V���O���X���b�h�Ȃ̂ŁA�����ˁI
	}
}

void	CAppBase::InvalidateThread(void){	//	overriden from CThread
//	Idle���[�h���̗p���Ă���̂ŁA���b�Z�[�W�����Ȃ�����A
//	IsThreadValid���Ăяo����Ȃ��A���Ȃ킿�AWM_CLOSE�ɉ������ĕ��邱�Ƃ��o���Ȃ�
	if (m_bIdle){
		HWND hWnd = GetHWnd();
		if (hWnd!=NULL) {	//	���̃��b�Z�[�W�����Ĕj�󂵂Ă܂�
			//	::SendMessage(hWnd,WM_CLOSE,0,0);
			//	�����́A���b�Z�[�W�L���[�ɐς܂�邽�߁A
			//	������Ăяo�����X���b�h�̃��b�Z�[�W���[�v���Ă΂��
			//	�܂ŁA����̎��s���x�������B
			//	����āA
			::PostMessage(hWnd,WM_CLOSE,0,0);
			//	���ꂪ�����B
		}
	}
	CThread::InvalidateThread();	//	super�N���X�ɈϏ�����
}

//////////////////////////////////////////////////////////////////////////////
//	���̃N���X�̃��b�Z�[�W�����p
//		�V���Ƀ��b�Z�[�W�������������Ƃ��́ACWinHook����h���������N���X������āA
//		�����Ńt�b�N�������Ă˂�B
LRESULT CAppBase::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){

	switch (uMsg){

	///////////////////////////////////////////////////////////////////////////
/*
	//	GetMessage�Ŏ����̑�����E�B���h�D���b�Z�[�W�������o���Ă��Ȃ��̂ŁA
	//	�A�v�����؂�ւ�����Ƃ���WM_PAINT���E���Ȃ����Ƃ�����悤�����c(Windows2000���̃o�O?)
	case WM_ACTIVATEAPP : {
		if( wParam ) UpdateWindow(hWnd);
		break;
						  }
*/
	case WM_CLOSE: { // �E�C���h�E������ꂽ
		if (!m_bClose && OnPreClose()) return 1; // �����������Ƃɂ��ċA��
		InvalidateThread();
//		InnerStopThread();		//	�����Worker���~�����Ă���łȂ���
								//	WM_DESTORY��hWnd�������ɂȂ�ƁA���̓r�[�A
								//	���[�J�[�X���b�h�����邱�ƂɂȂ�
		return 1;
		//	invalidate���Ă����΁A���[�J�[�X���b�h�͎����I�ɋA�҂���
		//	�A�҂������[�J�[�X���b�h��WM_DESTROY�𔭍s���Ă��炤
				   }

	case WM_DESTROY:{

		if (IsMainApp()) {
			PostQuitMessage(0); //	���C���E�B���h�D�������Ȃ�I������
		}
		break;
					}
	}
	return 0;
}

void	CAppBase::Close(void){
	m_bClose=true;
	::SendMessage(GetHWnd(),WM_CLOSE,0,0);
}