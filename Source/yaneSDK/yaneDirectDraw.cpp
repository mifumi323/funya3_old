// DirectDraw Wrapper

#include "stdafx.h"
#include "yaneDirectDraw.h"
#include "yaneCOMBase.h"	// COM�̗��p�̂���
#include "yaneAppInitializer.h"
#include "yaneAppManager.h"
#include "yaneWMUSER.h"
#include "yanePlane.h"
#include "yaneDIB32.h"
#include "yaneWindow.h"
#include "yaneTimer.h"

//////////////////////////////////////////////////////////////////////////////
//	CDirectDrawBase : DIRECTDRAW�C���^�[�t�F�[�X��p�ӂ���
//////////////////////////////////////////////////////////////////////////////

#if defined(USE_DirectDraw) || defined(USE_FastDraw) || defined(USE_DIB32)

////	singleton�o�[�W����

int				CDirectDrawBase::m_nRef = 0;
LPDIRECTDRAW	CDirectDrawBase::m_lpDirectDraw = NULL;

CDirectDrawBase::CDirectDrawBase(bool bEmulationOnly){
	if (m_nRef++ == 0) {
		if (CCOMBase::AddRef()) { // COM�g���Ł[
			//	���ꎸ�s����񂩁[�H���[���������m����O�O
			m_lpDirectDraw = NULL;
		} else {
			Initialize(bEmulationOnly);
		}
	}
}

CDirectDrawBase::~CDirectDrawBase(){
	if (--m_nRef == 0){
		Terminate(); // �s���Ȏ�҂̂��߂Ɉꉞ�A�I������:p
		CCOMBase::Release(); // COM�g���I������Ł[
	}
}

LRESULT CDirectDrawBase::Initialize(bool bEmulationOnly){
	// COM�I�ďo��
	if (::CoCreateInstance(CLSID_DirectDraw,
			NULL, CLSCTX_ALL, IID_IDirectDraw, (LPVOID*)&m_lpDirectDraw)!=DD_OK){
		Err.Out("CDirectDrawBase::Initialize��CoCreateInstance�Ɏ��s");
		m_lpDirectDraw = NULL;
		return 1;
	}

	if (bEmulationOnly) {
		if (m_lpDirectDraw->Initialize((GUID*)DDCREATE_EMULATIONONLY)!=DD_OK) {
			//--- �C�� '02/04/26  by KAIZA ---
			//	�ꉞ�AHAL���������g���C����
			if (m_lpDirectDraw->Initialize(NULL)!=DD_OK) {
				Err.Out("CDirectDrawBase::Initialize��HAL�������Ɏ��s");
				m_lpDirectDraw = NULL;
				return 1;
			}
			//--------------------------------
		}
	} else {
		if (m_lpDirectDraw->Initialize(NULL)!=DD_OK) {
			//--- �C�� '02/04/26  by KAIZA ---
			Err.Out("CDirectDrawBase::Initialize��HAL�������Ɏ��s");
			//--------------------------------
			m_lpDirectDraw = NULL;
			return 1;
		}
	}
	return 0;
}

LRESULT CDirectDrawBase::Terminate(void){
	RELEASE_SAFE(m_lpDirectDraw);
	return 0;
}

////	������͔�singleton�o�[�W����
//	�i�eIDirectDraw���ƂɃv���C�}���T�[�t�F�[�X����������Ȃ��̂�
//		����Ȃ��Ƃ����Ȃ��Ƃ����Ȃ��Ȃ�j

CDirectDrawBaseM::CDirectDrawBaseM(bool bEmulationOnly){
	if (CCOMBase::AddRef()) { // COM�g���Ł[
			//	���ꎸ�s����񂩁[�H���[���������m����O�O
		m_lpDirectDraw = NULL;
	} else {
		Initialize(bEmulationOnly);
	}
}

CDirectDrawBaseM::~CDirectDrawBaseM(){
	Terminate(); // �s���Ȏ�҂̂��߂Ɉꉞ�A�I������:p
	CCOMBase::Release(); // COM�g���I������Ł[
}

LRESULT CDirectDrawBaseM::Initialize(bool bEmulationOnly){
	// COM�I�ďo��
	if (::CoCreateInstance(CLSID_DirectDraw,
			NULL, CLSCTX_ALL, IID_IDirectDraw, (LPVOID*)&m_lpDirectDraw)!=DD_OK){
		Err.Out("CDirectDrawBaseM::Initialize��CoCreateInstance�Ɏ��s");
		m_lpDirectDraw = NULL;
		return 1;
	}

	if (bEmulationOnly) {
		if (m_lpDirectDraw->Initialize((GUID*)DDCREATE_EMULATIONONLY)!=DD_OK) {
			//--- �C�� '02/04/26  by KAIZA ---
			//	�ꉞ�AHAL���������g���C����
			if (m_lpDirectDraw->Initialize(NULL)!=DD_OK) {
				Err.Out("CDirectDrawBaseM::Initialize��HAL�������Ɏ��s");
				m_lpDirectDraw = NULL;
				return 1;
			}
			//--------------------------------
		}
	} else {
		if (m_lpDirectDraw->Initialize(NULL)!=DD_OK) {
			//--- �C�� '02/04/26  by KAIZA ---
			Err.Out("CDirectDrawBaseM::Initialize��HAL�������Ɏ��s");
			//--------------------------------
			m_lpDirectDraw = NULL;
			return 1;
		}
	}
	return 0;
}

LRESULT CDirectDrawBaseM::Terminate(void){
	RELEASE_SAFE(m_lpDirectDraw);
	return 0;
}


#endif // DirectDrawBase

//////////////////////////////////////////////////////////////////////////////
//	CBppManager ����́A�ėp�Ȃ̂�#ifdef�`endif�͕s�v

int CBppManager::m_nBpp = -1;
int CBppManager::GetBpp() {
	if (m_nBpp==-1) Reset();
	return m_nBpp;
}
void CBppManager::Reset() {
	//	�𑜓x�ύX�������ꍇ�́A���̊֐����Ăяo��

	HDC		hdc;
	hdc = ::GetDC(NULL);

	m_nBpp = ::GetDeviceCaps(hdc, PLANES) * GetDeviceCaps(hdc, BITSPIXEL);
	if (m_nBpp==0){
		WARNING(true,"CPlane::InnerGetBpp��Bpp���[��...");
		::Sleep(1000);	//	�r�f�I�J�[�h�ύX�̗]�g���󂯂Ă���̂��H
		m_nBpp = ::GetDeviceCaps(hdc, PLANES) * GetDeviceCaps(hdc, BITSPIXEL);
	}

	::ReleaseDC(NULL, hdc);
}

//////////////////////////////////////////////////////////////////////////////
//	CDirectDrawSurfaceManager
//	�s�N�Z���t�H�[�}�b�g���ׂ�˂�I
#if defined(USE_DirectDraw) || defined(USE_FastDraw)

int CDirectDrawSurfaceManager::GetSurfaceType(){
	return m_nType;
}

void CDirectDrawSurfaceManager::OnChangeSurface(LPDIRECTDRAWSURFACE pSurface){
	m_nType = GetSurfaceType(pSurface);
}

int CDirectDrawSurfaceManager::GetSurfaceType(LPDIRECTDRAWSURFACE pSurface){
	//	����ʂ�bpp���ς�����Ƃ��́A���̊֐��Ăяo���ĂˁI
	if (pSurface==NULL) return -1;

	DDSURFACEDESC dddesc = { sizeof(dddesc) };
	//--- �C�� '02/03/19  by ENRA ---
//*
	// PixelFormat�𓾂�̂ɁALock����K�v�͂Ȃ����AMillenium G400/450�Ȃǂ�
	// Primary��Lock���邱�Ƃ��ł��Ȃ��B
	dddesc.ddsCaps.dwCaps = DDSD_PIXELFORMAT;
	if(DD_OK!=pSurface->GetSurfaceDesc( &dddesc )){
		Err.Out("CDirectDrawSurfaceManager::GetSurfaceType��GetSurfaceDesc�Ɏ��s");
		return 1;	//	����Ɏ��s
	}
/*/
	LRESULT hres;
	while ((hres = pSurface->Lock(NULL, &dddesc, 0, NULL)) == DDERR_WASSTILLDRAWING) ;
	if (hres !=DD_OK){
		::Sleep(2000);	//	�r�f�I�J�[�h�ύX�̗]�g���󂯂Ă���̂��H
		while ((hres = pSurface->Lock(NULL, &dddesc, 0, NULL)) == DDERR_WASSTILLDRAWING) ;
		if (hres !=DD_OK){
			Err.Out("CDirectDrawSurfaceManager::Surface��Lock�Ɏ��s");
			return 1;	//	����Ɏ��s
		}
	}
//*/
	//-------------------------------
	int nBpp = dddesc.ddpfPixelFormat.dwRGBBitCount;
	DWORD	RMask = dddesc.ddpfPixelFormat.dwRBitMask;
	DWORD	GMask = dddesc.ddpfPixelFormat.dwGBitMask;
	DWORD	BMask = dddesc.ddpfPixelFormat.dwBBitMask;

	int nType;
	switch (nBpp){
	case 8:
		nType = 2;	//	8bpp
		break;
	case 16:
		if		(RMask == 0xf800 && GMask == 0x07e0 && BMask == 0x001f) {
			nType = 3;	// RGB565
		} ef	(RMask == 0x7c00 && GMask == 0x03e0 && BMask == 0x001f) {
			nType = 4;	// RGB555
		} else {
			Err.Out("CDirectDrawSurfaceManager::16bpp�̕s���s�N�Z���t�H�[�}�b�g���o");
			Err.Out("BMask=0x%x BMask=0x%x BMask=0x%x",RMask,GMask,BMask);
			nType = 1;	//	16bpp�̕s���s�N�Z���t�H�[�}�b�g�D�D
		}
		break;
	case 24:
		if		(RMask == 0xff0000 && GMask == 0xff00 && BMask == 0xff) {
			nType = 5;	// RGB888
		}	ef	(RMask == 0xff	   && GMask == 0xff00 && BMask == 0xff0000) {
			nType = 6;	// BGR888
		} else {
			Err.Out("CDirectDrawSurfaceManager::24bpp�̕s���s�N�Z���t�H�[�}�b�g���o");
			Err.Out("BMask=0x%x BMask=0x%x BMask=0x%x",RMask,GMask,BMask);
			nType = 1;	//	24bpp�̕s���s�N�Z���t�H�[�}�b�g�D�D
		}
		break;
	case 32:
		if		(RMask == 0xff0000 && GMask == 0xff00 && BMask == 0xff) {
			nType = 7;	// XRGB888
		}	ef	(RMask == 0xff	   && GMask == 0xff00 && BMask == 0xff0000) {
			nType = 8;	// XBGR888
		} else {
			Err.Out("CDirectDrawSurfaceManager::32bpp�̕s���s�N�Z���t�H�[�}�b�g���o");
			Err.Out("BMask=0x%x BMask=0x%x BMask=0x%x",RMask,GMask,BMask);
			nType = 1;	//	32bpp�̕s���s�N�Z���t�H�[�}�b�g�D�D
		}
		break;
	default:
		Err.Out("CDirectDrawSurfaceManager::bpp�s���̃s�N�Z���t�H�[�}�b�g���o");
		Err.Out("nBpp=%d BMask=0x%x BMask=0x%x BMask=0x%x",nBpp,RMask,GMask,BMask);
		nType = 1;	//	�Ȃ�ł��́A����H�O�O�G
		break;
	}
//	pSurface->Unlock(NULL);
	//	�I���ł���A�I���`��(���ށ�)o
	return nType;
}

int CDirectDrawSurfaceManager::m_nType=0;

//	CWindowClipper
#ifdef USE_DirectDraw
LRESULT CWindowClipper::Clip(CPlane*lpPrimary,HWND hWnd){
	//	Window���[�h�łȂ���ΈӖ��͖���

	LPDIRECTDRAW lpDraw = CAppManager::GetMyDirectDraw()->GetDDraw();
	if (lpDraw==NULL) return 1;

	if (lpDraw->CreateClipper(0,&m_lpClipper,NULL)!=DD_OK){
		Err.Out("CWindowClipper::Clip��Clipper�\�z���s");
		return 2;
	}
	if (m_lpClipper->SetHWnd(0,hWnd)!=DD_OK){
		Err.Out("CWindowClipper::Clip��hWND���Z�b�g�ł��Ȃ�");
		return 3;
	}
	if (lpPrimary->GetSurface()->SetClipper(m_lpClipper)!=DD_OK){
		Err.Out("CWindowClipper::Clip��SetClipper�Ɏ��s");
		return 4;
	}

	return 0;
}
#endif // CWindowClipper::Clip(CPlane*lpPrimary,HWND hWnd)

void	CWindowClipper::Release(void){
	RELEASE_SAFE(m_lpClipper);
}

CWindowClipper::CWindowClipper(void){
	m_lpClipper = NULL;
}

CWindowClipper::~CWindowClipper(){
	Release();
}
#endif	//	CWindowClipper

//////////////////////////////////////////////////////////////////////////////
//	CDirectDraw : ��ʉ𑜓x��؂�ւ���Ȃ�
//////////////////////////////////////////////////////////////////////////////
#ifdef	USE_DirectDraw

CDirectDraw::CDirectDraw(void) {
	m_nScreenXSize	= 640;
	m_nScreenYSize	= 480;
	m_nScreenColor	= 16;
	m_bFullScr		= false;
	m_bUseFlip		= true;

	m_nSecondaryOffsetX = 0;
	m_nSecondaryOffsetY = 0;

	m_nBrightness	= 256;
	m_bLostSurface	= false; 

#ifdef USE_DIB32
	m_lpSecondaryDIB = NULL;
#endif

	m_hWnd	= CAppInitializer::GetHWnd();
	CAppManager::Add(this);
	CAppInitializer::Hook(this);		//	�t�b�N���J�n����

	//	�`��E�F�C�g�̂��߂ɁA�ŏ��^�C���C���^�[�o���ɐݒ肷��
	CTimeBase::timeBeginPeriodMin();
}

CDirectDraw::~CDirectDraw(){

	//	�`��E�F�C�g�̍ŏ��^�C���C���^�[�o������������
	CTimeBase::timeEndPeriodMin();

#ifdef USE_DIB32
	DELETE_SAFE(m_lpSecondaryDIB);		//	���������Ƃ��Ȃ���
#endif
	CAppInitializer::Unhook(this);		//	�t�b�N����������B
	CAppManager::Del(this);
}

//////////////////////////////////////////////////////////////////////////////

void CDirectDraw::BeginChangeDisplay(void){
	m_bDirty		= true;

	m_bDisplayChanging = true;
	m_bChangeDisplayMode = false;
	if (GetDDraw()==NULL) return ;

/*
	CPlane::ReleaseBufferAll();
	// �ǂݍ���ł����t�@�C���������͎c��

	if (m_ScreenMode!=FullScreenMode || !m_bUseDirectDrawFlip2) {
	// FullScreenMode��Flip�p�ɃA�^�b�`���Ă���A�������K�v�Ȃ��B
		RELEASE_SAFE(m_lpSecondary);
	} else {
		m_lpSecondary = NULL;
	}

	// �p���b�g�̃f�^�b�`�i�A�^�b�`���Ă���Palette�̉���j
	if (m_lpPrimary!=NULL && m_lpPrimary->SetPalette(NULL)== DDERR_SURFACELOST ) {
		m_lpPrimary->Restore(); // ���̃G���[�`�F�b�N��...�����ł��傤...
		m_lpPrimary->SetPalette(NULL);
	}
	
	RELEASE_SAFE(m_lpClipper);
	RELEASE_SAFE(m_lpPrimary);
	RELEASE_SAFE(m_lpPaletteMain);
*/

	// FullScreenMode��Flip�p�ɃA�^�b�`���Ă���A�������K�v�Ȃ�����...
	//	�Z�J���_���A�N���b�p�A�v���C�}���̉��

	/*
	if (m_bUseFlip) {
		LPDIRECTDRAWSURFACE lp = m_Primary.GetSurface();
		LRESULT hr;
		if (lp!=NULL) {
			m_Primary.Restore();
			m_Secondary.Restore();
	
			if ((hr=lp->DeleteAttachedSurface(0,m_Secondary.GetSurface()))!=DD_OK){
				Err.Out("CDirectDraw::Surface�̃f�^�b�`�Ɏ��s %d",hr);	
				//	���ƁADDERR_SURFACELOST���Ԃ��Ă��āA�f�^�b�`�Ɏ��s����
				//	Err.Out("DDERR_SURFACELOST          %d",DDERR_SURFACELOST);
		}
	}
	*/
 
	m_Secondary.Release();
	m_WindowClipper.Release();
	m_Primary.Release();

	if (m_bFullScr) {	//	�����t���X�N���[�����[�h�Ȃ�΂����߂��K�v������
		if (GetDDraw()->SetCooperativeLevel(m_hWnd,DDSCL_NORMAL)){
			Err.Out("CDirectDraw::BeginChangeDisplay��SetCooperativeLevel�Ɏ��s..");
		}
		// �E�B���h�D���[�h�ɂ���Ƃ��́A������s�Ȃ��K�v����
		if (GetDDraw()->RestoreDisplayMode()){
			Err.Out("CDirectDraw::BeginChangeDisplay��RestoreDisplayMode�Ɏ��s..");
		}
	}

}

void CDirectDraw::TestDisplayMode(int nSX,int nSY,bool bFullScr,int nColor){
	if (!m_bDisplayChanging) return ; // already changing!!
	if (GetDDraw()==NULL) return ;

	if (!bFullScr) {
		// Window���[�h�̎w�肪����Ȃ�Ύd�����Ȃ��B
		if (GetDDraw()->SetCooperativeLevel(m_hWnd,DDSCL_NORMAL)){
			Err.Out("CDirectDraw::TestDisplayMode��SetCooperativeLevel��DDSC_NORMAL�Ŏ��s");
			return ; // ����ł����񂩂�����͂����茾���ăV�����Ȃ��̂₯��...
		}
	} else {
		if (GetDDraw()->SetCooperativeLevel(m_hWnd,DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN)!=DD_OK){
			Err.Out("CDirectDraw::TestDisplayMode��SetCooperativeLevel��DDSC_EXCLUSIVE�Ŏ��s");
			return ; // ���̃A�v���g���Ă񂿂Ⴄ��H
		}
		if (GetDDraw()->SetDisplayMode(nSX,nSY,nColor)!=DD_OK){
			Err.Out("CDirectDraw::TestDisplayMode��SetDisplayMode�Ɏ��s");
			return ; // ��Ή����H
		}
	}

	// �E�C���h�E���[�h�Ńf�B�X�v���C���[�h��ύX���Ȃ���
	m_bChangeDisplayMode |= (m_bFullScr!=bFullScr) || m_bFullScr;// �ǉ�

	//	��ʃ��[�h�𔽉f������...
	m_bFullScr	= bFullScr;

	m_bUseFlip2 = m_bUseFlip;
	//	�v���C�}���T�[�t�F�[�X�̍쐬
	if (m_Primary.CreatePrimary(m_bUseFlip2,nSX,nSY)!=0) {
		Err.Out("CDirectDraw::TestDisplayMode��CreatePrimary�Ɏ��s");
		return ;
	}

	//	�E�B���h�D���[�h�Ȃ�΃N���b�p���d�|����
	m_WindowClipper.Release();
	if (!bFullScr){
		if (m_WindowClipper.Clip(&m_Primary,m_hWnd)!=0) {
			Err.Out("CDirectDraw::TestDisplayMode��Clip�Ɏ��s");
			return ;
		}
	}

	//	�Z�J���_���T�[�t�F�[�X�̍쐬
	if (m_Secondary.CreateSecondary(&m_Primary,m_bUseFlip2)!=0) {
		Err.Out("CDirectDraw::TestDisplayMode��CreateSecondary�Ɏ��s");
		return ;
	}

/*
	// 256�łȂ��Ȃ�΃p���b�g�̐ݒ�͖��Ӗ�
	if (m_nScreenColorMode2==8) InstallPalette();
*/
	//	�����ŃE�B���h�D���[�h�𔽉f���Ă���
	CWindow::ChangeScreen(bFullScr);
	CAppManager::GetMyApp()->GetMyWindow()->ChangeWindowStyle();

	m_bDisplayChanging	= false;	//	��ʃ��[�h�̕ύX�ɐ����I
	m_nScreenColor	= nColor;
	m_nScreenXSize	= nSX;
	m_nScreenYSize	= nSY;
	return ;
}

LRESULT CDirectDraw::EndChangeDisplay(void){
	if (GetDDraw()==NULL) return 1;
	
	if (m_bDisplayChanging) { // Change����Ă��Ȃ��I�I
		Err.Out("CDirectDraw::BeginChangeDisplay�`EndChangeDisplay�Ɏ��s");
		return 1;
	}

	// �E�C���h�E���[�h�ŁA�f�B�X�v���C���[�h���ς���ĂȂ��Ȃ�҂��Ȃ�
	if(m_bChangeDisplayMode) {// �ǉ�
		::Sleep(500);	//	�r�f�I���[�h�̕ύX�̗]�g���󂯂�\��������
	}

	//	�v���[����bpp�Ď擾
	CPlane::InnerGetBpp();

/*
	// �p���b�g�̍č\�z
	m_bBrightnessChanged = true;
*/

	// ���O�ŕ`�悷��Ȃ�Ί֌W�Ȃ��̂���...
	CAppManager::GetMyApp()->GetMyWindow()->ShowCursor(!m_bFullScr);
	// �E�B���h�D�T�C�Y�̕ύX
	CAppManager::GetMyApp()->GetMyWindow()->SetSize(m_nScreenXSize,m_nScreenYSize);
	// �E�B���h�D�X�^�C���̕ύX
	CAppManager::GetMyApp()->GetMyWindow()->ChangeScreen(m_bFullScr);
	//	�����̏����E�B���h�D�ɂ͍��m���Ă��B
	//	�i�{���́A���ׂẴE�B���h�E�Ƀu���[�h�L���X�g����K�v������j
	CAppManager::GetMyApp()->GetMyWindow()->ChangeWindowStyle();

	// �v���[���̍č\�z�i�����������Ă���΁j
	CPlane::RestoreAll();

#ifdef USE_DIB32
	//	DIB�̕��A����
	if (m_lpSecondaryDIB!=NULL){
		m_lpSecondaryDIB->Resize(m_nScreenXSize,m_nScreenYSize);
	}
#endif

	return 0;
}

/////// �g�p��:p) ////////////////////////////////////////////////////////////

LRESULT CDirectDraw::ChangeDisplay(bool bFullScr){
	if (GetDDraw()==NULL) return 1; //	���߂������...
	BeginChangeDisplay();
		if (bFullScr) {
			TestDisplayMode(m_nScreenXSize,m_nScreenYSize,true,m_nScreenColor);
		}
		TestDisplayMode(m_nScreenXSize,m_nScreenYSize); // �E�B���h�D���[�h
	if (EndChangeDisplay()) return 2;	//	�f�B�X�v���C���[�h�ύX�Ɏ��s
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//	�f�B�X�v���C���[�h�̐ݒ聨�ύX

LRESULT		CDirectDraw::SetDisplay(bool bFullScr,int nSizeX,int nSizeY,int nColorDepth){
	if (nSizeX!=0) {
		m_nScreenXSize	=	nSizeX;
	}
	if (nSizeY!=0) {
		m_nScreenYSize	=	nSizeY;
	}
	if (nColorDepth!=0) {
		m_nScreenColor	=	nColorDepth;
	} else {
		m_nScreenColor	=	16; // fixed '01/04/08
	}
	ChangeDisplay(bFullScr);
	//	���̃p�����[�^�����͂����ɓn��
	//	�im_bFullScr�͌��݂̉�ʃ��[�h��ێ����Ă���̂Łj
	return 0;
}

void		CDirectDraw::GetDisplay(bool&bFullScr,int &nSizeX,int &nSizeY,int &nColorDepth){
	bFullScr		=	m_bFullScr;
	nSizeX			=	m_nScreenXSize;
	nSizeY			=	m_nScreenYSize;
	nColorDepth		=	m_nScreenColor;
}

void		CDirectDraw::GetSize(int &nSizeX,int &nSizeY){	// GetDisplayMode��x,y�����������
	nSizeX			=	m_nScreenXSize;
	nSizeY			=	m_nScreenYSize;
}
	
bool		CDirectDraw::IsFullScreen(void){
	return		m_bFullScr;
}

void		CDirectDraw::SetFlipUse(bool bFlip){
	m_bUseFlip	= bFlip;
}

bool		CDirectDraw::GetFlipUse(void){
	return m_bUseFlip2;
}

void		CDirectDraw::FlipToGDISurface(void){
	if (CAppInitializer::IsFullScreen() && m_bUseFlip2) {
		if (GetDDraw()!=NULL) {
		   GetDDraw()->FlipToGDISurface();
		}
	}
}

void		CDirectDraw::CheckSurfaceLost(void){
	LPDIRECTDRAWSURFACE lpSurface = GetSecondary()->GetSurface();
	if (lpSurface==NULL) return ;
	if (lpSurface->IsLost()){
		ChangeDisplay(m_bFullScr);
		//	�v���C�}���T�[�t�F�[�X�̍č\�z����s�Ȃ��ׂ�
	}
}

int			CDirectDraw::GetBpp(void){	// ���݂�Bpp�̎擾
	return CPlane::GetBpp();
}

void	CDirectDraw::SetBrightness(int nBright){
	if (m_nBrightness == nBright) return ;
	m_nBrightness = nBright;
	m_bDirty = true;
}

#ifdef USE_DIB32
//	DIB��Lost���Ȃ��̂ŕ��A�������͈�ؕs�v�B�������Ȃ��i�΁j
LRESULT		CDirectDraw::CreateSecondaryDIB(void){
	m_lpSecondaryDIB = new CDIB32;
	if (m_lpSecondaryDIB==NULL) return 1;	//	�����������̂��H�O�O
	m_lpSecondaryDIB->UseDIBSection(true);	//	GetDC�̉\�ȃT�[�t�F�[�X

	//	�����ŃT�C�Y���v�������̂ŁA�f�B�X�v���C���[�h�̕ύX���������Ă��邱�ƁB
	if (m_lpSecondaryDIB->CreateSurface(m_nScreenXSize,m_nScreenYSize)){
		//	�����������̂��H�O�O
		DELETE_SAFE(m_lpSecondaryDIB);
		return 2;
	}
	return 0;
}

void		CDirectDraw::ReleaseSecondaryDIB(void){
	DELETE_SAFE(m_lpSecondaryDIB);
}
#endif

//////////////////////////////////////////////////////////////////////////////
//	�]���n�@�i�����͂��ׂ�CPlane�ɈϏ�����j
LRESULT		CDirectDraw::Blt(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect,LPRECT lpClipDstRect){
	m_bDirty		= true;
	return GetSecondary()->Blt(lpSrc,x,y,lpSrcRect,lpClipDstRect);
}

LRESULT		CDirectDraw::BltFast(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect,LPRECT lpClipDstRect){
	m_bDirty		= true;
	return GetSecondary()->BltFast(lpSrc,x,y,lpSrcRect,lpClipDstRect);
}

LRESULT		CDirectDraw::BlendBlt(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect,LPRECT lpClipDstRect){
	m_bDirty		= true;
	return GetSecondary()->BlendBlt(lpSrc,x,y,ar,ag,ab,br,bg,bb,lpSrcRect,lpClipDstRect);
}

LRESULT		CDirectDraw::BlendBltFast(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect,LPRECT lpClipDstRect){
	m_bDirty		= true;
	return GetSecondary()->BlendBltFast(lpSrc,x,y,ar,ag,ab,br,bg,bb,lpSrcRect,lpClipDstRect);
}

LRESULT		CDirectDraw::BltR(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipDstRect){
	m_bDirty		= true;
	return GetSecondary()->BltR(lpSrc,x,y,lpSrcRect,lpDstSize,lpClipDstRect);
}

LRESULT		CDirectDraw::BltFastR(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipDstRect){
	m_bDirty		= true;
	return GetSecondary()->BltFastR(lpSrc,x,y,lpSrcRect,lpDstSize,lpClipDstRect);
}

LRESULT		CDirectDraw::BlendBltR(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipDstRect){
	m_bDirty		= true;
	return GetSecondary()->BlendBltR(lpSrc,x,y,ar,ag,ab,br,bg,bb,lpSrcRect,lpDstSize,lpClipDstRect);
}

LRESULT		CDirectDraw::BlendBltFastR(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipDstRect){
	m_bDirty		= true;
	return GetSecondary()->BlendBltFastR(lpSrc,x,y,ar,ag,ab,br,bg,bb,lpSrcRect,lpDstSize,lpClipDstRect);
}

#ifdef USE_DIB32
LRESULT		CDirectDraw::Blt(CDIB32*lpSrc,int x,int y,LPRECT lpSrcRect,LPRECT lpClipRect){
	m_bDirty		= true;
	return GetSecondary()->Blt(lpSrc,x,y,lpSrcRect,lpClipRect);
}
#endif

	////////////////////////////////////////////////
	//	��������S���Ϗ������܂��O�O

LRESULT		CDirectDraw::Clear(LPRECT lpRect){
	m_bDirty		= true;
	return GetSecondary()->Clear(lpRect);
}

LRESULT		CDirectDraw::SetFillColor(COLORREF c){
	return GetSecondary()->SetFillColor(c);
}

DWORD		CDirectDraw::GetFillColor(void){
	return GetSecondary()->GetFillColor();
}

//////////////////////////////////////////////////////////////////////////////
//	CPlaneBase�̃����o�֐��B�S���A�Ϗ��O�O�G
LRESULT CDirectDraw::Blt(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return static_cast<CPlaneBase*>(GetSecondary())->Blt(lpSrc,x,y,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDirectDraw::BltFast(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return static_cast<CPlaneBase*>(GetSecondary())->BltFast(lpSrc,x,y,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDirectDraw::BlendBlt(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
						   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return static_cast<CPlaneBase*>(GetSecondary())->BlendBlt(lpSrc,x,y,dwDstRGBRate,dwSrcRGBRate,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDirectDraw::BlendBltFast(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
							   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return static_cast<CPlaneBase*>(GetSecondary())->BlendBltFast(lpSrc,x,y,dwDstRGBRate,dwSrcRGBRate,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDirectDraw::MosaicEffect(int d, LPRECT lpRect){
	return static_cast<CPlaneBase*>(GetSecondary())->MosaicEffect(d,lpRect);
}
LRESULT CDirectDraw::FlushEffect(LPRECT lpRect){
	return static_cast<CPlaneBase*>(GetSecondary())->FlushEffect(lpRect);
}
/*	//	CPlane::CreateSecondary����Q�Ƃ����̂ŁA����͂܂���
void	CDirectDraw::GetSize(int &x,int &y) {
	static_cast<CPlaneBase*>(GetSecondary())->GetSize(x,y);
}
*/
LRESULT CDirectDraw::ClearRect(LPRECT lpRect){
	return static_cast<CPlaneBase*>(GetSecondary())->ClearRect(lpRect);
}
LRESULT CDirectDraw::Load(string szBitmapFileName,bool bLoadPalette){
	return static_cast<CPlaneBase*>(GetSecondary())->Load(szBitmapFileName,bLoadPalette);
}
LRESULT CDirectDraw::LoadW(string szBitmapFileName256,string szBitmapFileNameElse
			,bool bLoadPalette){
	return static_cast<CPlaneBase*>(GetSecondary())->LoadW(
		szBitmapFileName256,szBitmapFileNameElse,bLoadPalette);
}
LRESULT CDirectDraw::Release(void){
	return static_cast<CPlaneBase*>(GetSecondary())->Release();
}
LRESULT CDirectDraw::SetColorKey(int x,int y){
	return static_cast<CPlaneBase*>(GetSecondary())->SetColorKey(x,y);
}
LRESULT CDirectDraw::SetColorKey(int r,int g,int b){
	return static_cast<CPlaneBase*>(GetSecondary())->SetColorKey(r,g,b);
}

bool CDirectDraw::IsLoaded(void) const {
	//	GetSecondary()�ł́Aconst����������̂Łc
	return m_Secondary.IsLoaded();
}

//////////////////////////////////////////////////////////////////////////////

void	CDirectDraw::SetOffset(int ox,int oy){
	//	�Z�J���_���̓]���I�t�Z�b�g
	m_nSecondaryOffsetX =	ox;
	m_nSecondaryOffsetY =	oy;
}

void	CDirectDraw::OnDraw(RECT*lpRect,bool bLayerCallBack){
	
	//	�����A�T�[�t�F�[�X���������Ă����...
	if (m_bLostSurface) {
		//	�X���b�h�������j�������̂Ȃ�΃��X�g�A�͖���...
		if (!CAppManager::GetMyApp()->IsThreadValid()) return ;
		CheckSurfaceLost();
		//	���̃t���O�́A���b�Z�[�W�|���v����
		//	���X�g�A�C�x���g�ɉ����Đݒ肳���
		m_bLostSurface = false;
		return ;
	}
	
	LPDIRECTDRAWSURFACE lpPrimary = GetPrimary()->GetSurface();
	if (lpPrimary==NULL) return ;
	LPDIRECTDRAWSURFACE lpSecondary = GetSecondary()->GetSurface();
	if (lpSecondary==NULL) return ;
	
	//////////////////////////////////////////////////////////
	//	Layer callback
	if ( bLayerCallBack ){
		//	Layer���Z�J���_���ɕ`�悵�Ă��炤�O�O
		m_LayerList.OnDraw(this);
		
		//	HDCLayer���Z�J���_���ɕ`�悵�Ă��炤�O�O
		if (!m_HDCLayerList.IsEmpty()){
			HDC hdc = GetSecondary()->GetDC();
			if (hdc!=NULL){
				m_HDCLayerList.OnDraw(hdc);
				GetSecondary()->ReleaseDC();
			}
			Invalidate();	//	��������������ŁA�����t���O���Ă�
		}
		
		//	AfterLayer���Z�J���_���ɕ`�悵�Ă��炤�O�O
		m_AfterLayerList.OnDraw(this);
	}
	//////////////////////////////////////////////////////////
	
	//	�v���C�}��������Ă��Ȃ���΃��^�[��
	if (!m_bDirty) return ;
	
	//	DirtyRect�Ǘ��͍s�Ȃ��Ă��Ȃ�
	//	�ǂ���FullScreen��Flip���g���ƍŏ��X�V�����ł͊Ǘ��ł��Ȃ�
	
	//	�u���C�g�l�X�ɉ����āA�Z�J���_��������
	RealizeBrightness(m_nBrightness);
	
	RECT r; // source rect
	::SetRect(&r,0,0,m_nScreenXSize,m_nScreenYSize);
	
	if (m_nSecondaryOffsetX || m_nSecondaryOffsetY) {
		//	�]����I�t�Z�b�g���������Ă���̂��H
		RECT r2; // distination rect
		if ( lpRect == NULL ){	// �����X�V���H
			::SetRect(&r2,0,0,m_nScreenXSize,m_nScreenYSize);	//	����T�C�Y�Ɖ��肵�ėǂ�
			//	a clipping algorithm
			//		between the same size rect (C) yaneurao'1999
			if (m_nSecondaryOffsetX>0) {
				r.right		-= m_nSecondaryOffsetX;
				r2.left		+= m_nSecondaryOffsetX;
			} else {
				r.left		-= m_nSecondaryOffsetX;
				r2.right	+= m_nSecondaryOffsetX;
			}
			if (m_nSecondaryOffsetY>0) {
				r.bottom	-= m_nSecondaryOffsetY;
				r2.top		+= m_nSecondaryOffsetY;
			} else {
				r.top		-= m_nSecondaryOffsetY;
				r2.bottom	+= m_nSecondaryOffsetY;
			}
			
			if (CAppInitializer::IsFullScreen()){
				// �t���X�N���[���ł��ABlt�Ȃ̂�I
				DDBLTFX fx;
				ZERO(fx);	//	�ꉞ������
				fx.dwSize = sizeof(fx);
				fx.dwFillColor = GetSecondary()->GetFillColor();
				RECT br;
				::SetRect(&br,0,0,m_nScreenXSize,r2.top);					//	��̈�
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				//	main�摜
				lpPrimary->BltFast(r2.left,r2.top,lpSecondary,&r,DDBLTFAST_WAIT);
				::SetRect(&br,0,r2.top,r2.left,r2.bottom);				//	���̈�
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				::SetRect(&br,r2.right,r2.top,m_nScreenXSize,r2.bottom);	//	�E�̈�
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				::SetRect(&br,0,r2.bottom,m_nScreenXSize,m_nScreenYSize);	//	���̈�
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				
			} else {
				// DirectDrawClipper�𗘗p����ɂ�Blt�ɂ��Ȃ��Ă͂Ȃ�Ȃ��B
				// �������A�Ȃ񂿂�[������B���������ɂ��ė~����>Clipper�N�I
				POINT cp;
				cp.x = cp.y = 0;
				::ClientToScreen(m_hWnd,&cp); // �N���C�A���g�E�B���h�D�̋�`������I
				
				DDBLTFX fx;
				ZERO(fx);	//	�ꉞ������
				fx.dwSize = sizeof(fx);
				fx.dwFillColor = GetSecondary()->GetFillColor();
				
				RECT br;
				::SetRect(&br,0,0,m_nScreenXSize,r2.top);					//	��̈�
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				
				//	main�摜
				br = r2;
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,lpSecondary,&r,DDBLT_WAIT,NULL);
				
				::SetRect(&br,0,r2.top,r2.left,r2.bottom);					//	���̈�
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				::SetRect(&br,r2.right,r2.top,m_nScreenXSize,r2.bottom);	//	�E�̈�
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				::SetRect(&br,0,r2.bottom,m_nScreenXSize,m_nScreenYSize);	//	���̈�
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
			}
		}else{
			// �����X�V
				r = *lpRect;	// source rest;
				r2 = *lpRect;	// dest	 rest;
			if (CAppInitializer::IsFullScreen()){
				// from CPlane DRAW_CLIPPER
				RECT& sr = r;			
				int x = m_nSecondaryOffsetX;
				int y = m_nSecondaryOffsetY;
				{
					RECT clip;
					::SetRect(&clip,0,0,m_nScreenXSize,m_nScreenYSize);
					int x2,y2;													
					x2 = x + sr.right - sr.left; /* x + Width  */				
					y2 = y + sr.bottom - sr.top; /* y + Height */				
					if (x2<clip.left || y2<clip.top								
						|| x>=clip.right || y>=clip.bottom) return;	 /* ��ʊO */ 
					int t;														
					t = clip.left-x;										
					if (t>0)	{ sr.left	+= t;	x = clip.left;	}		
					t = clip.top -y;										
					if (t>0)	{ sr.top	+= t;	y = clip.top;	}		
					t = x2-clip.right;										
					if (t>0)	{ sr.right	-= t;	x2= clip.right; }		
					t = y2-clip.bottom;										
					if (t>0)	{ sr.bottom -= t;	y2= clip.bottom;}		
					if (sr.right<=sr.left || sr.bottom<=sr.top) return; // invalid rect
				}
				lpPrimary->BltFast(x,y,lpSecondary,&r,DDBLTFAST_WAIT);
			}else{
				// �I�t�Z�b�g�]���̕����X�V�Ȃ̂ŁA���ꂽ���w�i�͕`�悵�Ȃ�
				// DirectDrawClipper�𗘗p����ɂ�Blt�ɂ��Ȃ��Ă͂Ȃ�Ȃ��B
				// �������A�Ȃ񂿂�[������B���������ɂ��ė~����>Clipper�N�I
				POINT cp;
				cp.x = cp.y = 0;
				::ClientToScreen(m_hWnd,&cp); // �N���C�A���g�E�B���h�D�̋�`������I
				::OffsetRect(&r2,m_nSecondaryOffsetX,m_nSecondaryOffsetY);
				::OffsetRect(&r2,cp.x,cp.y); 
				lpPrimary->Blt(&r2,lpSecondary,&r,DDBLT_WAIT,NULL);
			}
			
		}
		
	} else {
		//	�]���I�t�Z�b�g���w�肳��Ă��Ȃ��t�c�[�̏ꍇ
		
		if (CAppInitializer::IsFullScreen()){
			if (m_bUseFlip2 && lpRect == NULL ){
				lpPrimary->Flip(NULL,DDFLIP_WAIT);
			} else {
				if ( lpRect != NULL ){
					r = *lpRect;
				}
				lpPrimary->BltFast(0,0,lpSecondary,&r,DDBLTFAST_WAIT);
			}
		} else {
			// DirectDrawClipper�𗘗p����ɂ�Blt�ɂ��Ȃ��Ă͂Ȃ�Ȃ��B
			// �������A�Ȃ񂿂�[������B���������ɂ��ė~����>Clipper�N�I
			POINT cp;
			cp.x = cp.y = 0;
			::ClientToScreen(m_hWnd,&cp); // �N���C�A���g�E�B���h�D�̋�`������I
			if ( lpRect != NULL ) {
				r = *lpRect;
			}
			RECT sr = r;
			::OffsetRect(&sr,cp.x,cp.y);
			lpPrimary->Blt(&sr,lpSecondary,&r,DDBLT_WAIT,NULL);
		}
	}
	
	m_bDirty = false;
}
#ifdef USE_DIB32
//	��̊֐���DIB�o�[�W����
void	CDirectDraw::OnDrawDIB(RECT*lpRect,bool bLayerCallBack){
	//	DIB->Secondary�ɓ]�����āAOnDraw�ɈϏ��B
	//	�����x���Ȃ邪�A�ǂ���FullScreen�Ȃ�΃t���b�v���邵�A
	//	�������Ă�����WindowClipper����p���邵�A�v���O�����͊y�O�O
	m_lpSecondaryDIB->BltToPlane(GetSecondary(),m_nSecondaryOffsetX,m_nSecondaryOffsetY,lpRect);
	m_bDirty = true;
	OnDraw(lpRect,bLayerCallBack);
}
#endif

//////////////////////////////////////////////////////////////////////////////
void	CDirectDraw::RealizeBrightness(int nBrightness){
	if (GetBpp()==8) return ; // 256�F�͔�Ή�
	if (m_nBrightness==256) return ;

	// ���F���[�h�Ȃ̂ŃZ�J���_���𒼐ڏ�����������
	// �T�[�t�F�[�X�̒������I�i���ӋZ:p�j
	DDSURFACEDESC dddesc;
	ZERO(dddesc); // �ꉞ��
	dddesc.dwSize = sizeof(dddesc);
	if (GetSecondary()->GetSurface()->Lock(NULL,&dddesc,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("RealizeBrightness��Surface��Lock�Ɏ��s");
		return ;
	}

	LONG lPitch	 = dddesc.lPitch;

	// �s�N�Z���t�H�[�}�b�g���Q�Ƃ��Ȃ���
	// ���Ƃ�65536�F�\���ł�32�r�b�g�g�p���Ă���\��������
	switch (dddesc.ddpfPixelFormat.dwRGBBitCount) {
	case 16: { // 65536�F���[�h
		// DWORD���܂Ƃ߂Ėʓ|������I
		DWORD RMask, GMask, BMask;
		DWORD RMask2, GMask2, BMask2;

		RMask = dddesc.ddpfPixelFormat.dwRBitMask;
		GMask = dddesc.ddpfPixelFormat.dwGBitMask;
		BMask = dddesc.ddpfPixelFormat.dwBBitMask;
		RMask2 = dddesc.ddpfPixelFormat.dwRBitMask <<16;
		GMask2 = dddesc.ddpfPixelFormat.dwGBitMask <<16;
		BMask2 = dddesc.ddpfPixelFormat.dwBBitMask <<16;

		DWORD *p = (DWORD*)dddesc.lpSurface;
		
		for(int y=0;y<m_nScreenYSize;y++){
			for (int x=0; x<(m_nScreenXSize>>1);x++) {
				DWORD pixel, px;
				px	= p[x];
				pixel  = ((DWORD)(((px&RMask )*m_nBrightness)>>8)&RMask);
				pixel |= ((DWORD)(((px&GMask )*m_nBrightness)>>8)&GMask);
				pixel |= ((DWORD)(((px&BMask )*m_nBrightness)>>8)&BMask);
				pixel |= ((DWORD)(((px&RMask2)>>8)*m_nBrightness)&RMask2);
				pixel |= ((DWORD)(((px&GMask2)>>8)*m_nBrightness)&GMask2);
				pixel |= ((DWORD)(((px&BMask2)>>8)*m_nBrightness)&BMask2);
				p[x] = pixel;
			}
			p = (DWORD*)((BYTE*)p + lPitch); // �P���X�^���̑���
			// BYTE�ɃL���X�g���Ă����Ȃ��ƌv�Z�ԈႤ:p
		}
	} break;

	case 24: {
	  // ����ł�����B�����m���:p
	  // ���ۂ̂Ƃ���A�����Ԃ�x���C������B�iDWORD�P�ʂŏ����ł��Ȃ����߁j
	  // �܂�true color�̂ق��������}�V�B����́A����8x86�̏h���Ƃ��������悤���Ȃ��B

#if 0
		BYTE* p = (BYTE*)dddesc.lpSurface;
		// �F�ϊ��e�[�u������Ă܂��I
		// ���ꂪbyte�P�ʂ�����x���Ƃ����b�����邪...
		BYTE table[256];
		for(int i=0;i<256;i++){
			table[i] = (BYTE)(i * m_nBrightness >> 8);
		}

		DWORD width = m_nScreenXSize;
		for(int y=0;y<m_nScreenYSize;y++){
			/*
			for (int x=0;x<m_nScreenXSize;x++) {
				*p = table[*p]; p++; // �e�[�u������@(C)yaneurao
				*p = table[*p]; p++;
				*p = table[*p]; p++;
			}
			p += lPitch - m_nScreenXSize*3; // �P���X�^���̑���(bytes)
			*/

			// BYTE�A�N�Z�X���Ă���ϒx���Ȃ�
			_asm {
				mov ecx,width // Counter
				mov edx,p
				lea ebx,table
				xor eax,eax	 // reset
			slp:
				// STOSB���g�������ȁ[
				mov al,[edx]
				mov al,[ebx+eax]
				mov [edx],al
				inc edx
				mov al,[edx]
				mov al,[ebx+eax]
				mov [edx],al
				inc edx
				mov al,[edx]
				mov al,[ebx+eax]
				mov [edx],al
				inc edx
				loop slp
			}
			p += lPitch;
		}
#endif
//////////// �Ă��̂Q ///////////////////////////////////////////////////////
		BYTE table[256];
		for(int i=0;i<256;i++){
			table[i] = (BYTE)(i * m_nBrightness >> 8);
		}
		DWORD* p = (DWORD*)dddesc.lpSurface;
		int LP = m_nScreenXSize * 3 >> 2;
		for(int y=0;y<m_nScreenYSize;y++){
			_asm {
				push edi
				push esi
				lea edi,table
				mov esi,p
				mov ecx,LP	// loop counter
				xor ebx,ebx
			lpp:	// �������[�v�łS�o�C�g����������
				mov eax,[esi]
				mov bl,al
				mov dl,[edi+ebx]
				mov bl,ah
				mov dh,[edi+ebx]

				mov [esi],dx
				shr eax,16
				mov bl,al
				mov dl,[edi+ebx]
				shr eax,8
				mov dh,[edi+eax]

				mov [esi+2],dx
				add esi,4

				loop lpp
				pop esi
				pop edi
			}
		
			p = (DWORD*) ((BYTE*)p+ lPitch);
		}

		   } break;

	case 32: {
		DWORD RMask, GMask, BMask;

		RMask = dddesc.ddpfPixelFormat.dwRBitMask;
		GMask = dddesc.ddpfPixelFormat.dwGBitMask;
		BMask = dddesc.ddpfPixelFormat.dwBBitMask;

		DWORD *p = (DWORD*)dddesc.lpSurface;
		
		for(int y=0;y<m_nScreenYSize;y++){
			for (int x=0; x<m_nScreenXSize;x++) {
				DWORD pixel, px;
				px	= p[x];
				// �����ӂ�N�������[�H�V�����Ȃ��Ȃ�...
				// ULONGLONG�������o���̂͂�����Ƒ�l���Ȃ��悤�ȋC�����邯��:p
				pixel  = (DWORD)((((ULONGLONG)(px&RMask))*m_nBrightness)>>8)&RMask;
				pixel |= (DWORD)((((ULONGLONG)(px&GMask))*m_nBrightness)>>8)&GMask;
				pixel |= (DWORD)((((ULONGLONG)(px&BMask))*m_nBrightness)>>8)&BMask;
				p[x] = pixel;
			}
			p = (DWORD*)((BYTE*)p + lPitch); // �P���X�^���̑���
		}

	  } break; // end case
	} // end switch 
	GetSecondary()->GetSurface()->Unlock(NULL);
}

//////////////////////////////////////////////////////////////////////////////
LRESULT		CDirectDraw::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){

	switch(uMsg){
	case WM_PAINT : {
		//	dirty rect������
		m_bDirty = true;
//		OnDraw();	//	�ꉞ�A�ĕ`�悵�Ă܂��B
		/*
			����O���Ă����̂����A�����R�����g���O���Ƃ��Ă�
			WM_PAINT�̏������ɂ�GetDC�͎g���Ȃ��̂Œ��ӂ��K�v�B
			�iGetDC�Ŏ擾�����f�o�C�X�R���e�L�X�g�ɕ`�悷���WM_PAINT���������邩��j
			WM_PAINT�̏�������WM_PAINT�����������ŁA�A�v���P�[�V������WM_PAINT��
			���������ł��Ȃ��Ȃ�I
		*/

		return 0;	//	DefWindowProc���Ăяo���ɍs���B
					//	������Ăяo���Ă����Ȃ��ƍēxWM_PAINT�������Ă���
					}

	case WM_ACTIVATEAPP: {
		UINT bActive = wParam;
		if(bActive) {
			//	���b�Z�[�W�|���v���ł͂Ȃ��A���[�J�[�X���b�h���Ń��X�g�A���ׂ�...
			m_bLostSurface = true;
		}
		return 0;
						 }

	} // end switch

	return 0;
}

#endif // USE_DirectDraw