// DirectDraw Wrapper

#include "stdafx.h"

#ifdef USE_DIB32

#include "yaneDirectDraw.h"
#include "yaneAppInitializer.h"
#include "yaneAppManager.h"
#include "yaneWMUSER.h"
#include "yaneDIB32.h"
#include "yaneTimer.h"

//////////////////////////////////////////////////////////////////////////////

CDIBDraw::CDIBDraw(void) {
	m_nScreenXSize	= 640;
	m_nScreenYSize	= 480;
	m_nScreenColor	= 16;
	m_bFullScr		= false;

	m_nSecondaryOffsetX = 0;
	m_nSecondaryOffsetY = 0;

	{
	RECT rc,rc2;
	::SetRect(&rc,0,0,0,0);
	::SetRect(&rc2,0,0,0,0);
	::AdjustWindowRect(&rc,WS_CAPTION | WS_SYSMENU,true);
	::AdjustWindowRect(&rc2,WS_CAPTION ,false);
	//	�t���X�N���[�����̃��j���[�T�C�Y�𐳊m�ɎZ�o���邽�߁A���j���[�����𒲂ׂ�(c)yaneurao'00/12/15
	//	�i�Ȃ�ƃ��j���[�����͂n�r���ƂɈقȂ�O�O�G �j
	m_nMenu = (rc.bottom-rc.top) - (rc2.bottom-rc2.top);
	}

	m_nBrightness	= 256;

	m_hWnd	= CAppInitializer::GetHWnd();
	CAppManager::Add(this);
	CAppInitializer::Hook(this);		//	�t�b�N���J�n����

	//	�`��E�F�C�g�̂��߂ɁA�ŏ��^�C���C���^�[�o���ɐݒ肷��
	CTimeBase::timeBeginPeriodMin();
}

CDIBDraw::~CDIBDraw(){
	//	�`��E�F�C�g�̍ŏ��^�C���C���^�[�o������������
	CTimeBase::timeEndPeriodMin();

	CAppInitializer::Unhook(this);		//	�t�b�N����������B
	CAppManager::Del(this);
}


//////////////////////////////////////////////////////////////////////////////
//	�f�B�X�v���C���[�h�̐ݒ聨�ύX
//////////////////////////////////////////////////////////////////////////////
//	CDirectDraw����R�s�y�O�O�G

void CDIBDraw::BeginChangeDisplay(void){
	m_bDirty		= true;

	m_bDisplayChanging = true;
	m_bChangeDisplayMode = false;

#ifdef USE_DIRECTX // �ǉ�
	if (GetDDraw()==NULL) return ;

	// FullScreenMode��Flip�p�ɃA�^�b�`���Ă���A�������K�v�Ȃ�����...
	//	�Z�J���_���A�N���b�p�A�v���C�}���̉��

	if (m_bFullScr) {	//	���݁A�t���X�N���[�����[�h�Ȃ�΁A�����߂��K�v������
		if (GetDDraw()->SetCooperativeLevel(m_hWnd,DDSCL_NORMAL)){
			Err.Out("CDirectDraw::BeginChangeDisplay��SetCooperativeLevel�Ɏ��s..");
		}
		// �E�B���h�D���[�h�ɂ���Ƃ��́A������s�Ȃ��K�v����
		if (GetDDraw()->RestoreDisplayMode()){
			Err.Out("CDirectDraw::BeginChangeDisplay��RestoreDisplayMode�Ɏ��s..");
		}
	}
#endif
}

void CDIBDraw::TestDisplayMode(int nSX,int nSY,bool bFullScr,int nColor){
	if (!m_bDisplayChanging) return ; // already changing!!

#ifdef USE_DIRECTX	// �ǉ�
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
#endif

#ifndef USE_DIRECTX	//	�ǉ�
	WARNING(m_bFullScr==true,"�t���X�N���[���ɏo���Ȃ���");
#endif

	// �E�C���h�E���[�h�ł��ύX�Ȃ���
	m_bChangeDisplayMode |= (m_bFullScr!=bFullScr) || m_bFullScr;// �ǉ�

	//	��ʃ��[�h�𔽉f������...
	m_bFullScr	= bFullScr;

	m_lpSecondary.Add( new CDIB32 );
	if (m_lpSecondary==NULL) {
		Err.Out("CDIBDraw::TestDisplayMode��CreateSecondary�Ɏ��s");
		return ;	//	�����������̂��H�O�O
	}
	m_lpSecondary->UseDIBSection(true); //	GetDC�̉\�ȃT�[�t�F�[�X

	//	�����ŃT�C�Y���v�������̂ŁA�f�B�X�v���C���[�h�̕ύX���������Ă��邱�ƁB
	if (m_lpSecondary->CreateSurface(nSX,nSY)){
		//	�����������̂��H�O�O
		Err.Out("CDIBDraw::TestDisplayMode��CreateSecondary�Ɏ��s");
		m_lpSecondary.Delete();
		return ;
	}

	//	�����ŃE�B���h�D���[�h�𔽉f���Ă���
	CWindow::ChangeScreen(bFullScr);
	CAppManager::GetMyApp()->GetMyWindow()->ChangeWindowStyle();

	m_bDisplayChanging	= false;	//	��ʃ��[�h�̕ύX�ɐ����I
	m_nScreenColor	= nColor;
	m_nScreenXSize	= nSX;
	m_nScreenYSize	= nSY;
	return ;
}

LRESULT CDIBDraw::EndChangeDisplay(void){
#ifdef USE_DIRECTX	// �ǉ�
	if (GetDDraw()==NULL) return 1;
#endif

	if (m_bDisplayChanging) { // Change����Ă��Ȃ��I�I
		Err.Out("CDirectDraw::BeginChangeDisplay�`EndChangeDisplay�Ɏ��s");
		return 1;
	}

	// �f�B�X�v���C���[�h�ɕύX�������
	if(m_bChangeDisplayMode) {// �ǉ�
		::Sleep(500);	//	�r�f�I���[�h�̕ύX�̗]�g���󂯂�\��������
	}

	//	�v���[����bpp�Ď擾
	CBppManager::Reset();

/*
	// �p���b�g�̍č\�z
	m_bBrightnessChanged = true;
*/

	// ���O�ŕ`�悷��Ȃ�Ί֌W�Ȃ��̂���...
	// CAppManager::GetMyApp()->GetMyWindow()->ShowCursor(!m_bFullScr);
	//	��DIBDraw�Ȃ̂ŁA�`�悵�Ă��ǂ��͂����D�D

	// �E�B���h�D�T�C�Y�̕ύX
	CAppManager::GetMyApp()->GetMyWindow()->SetSize(m_nScreenXSize,m_nScreenYSize);
	//	����ChangeWindowStyle�ł��s�Ȃ��̂ŁA������Resize����Ɠ�d�ɂ�邱�ƂɂȂ�B

	// �E�B���h�D�X�^�C���̕ύX
	CAppManager::GetMyApp()->GetMyWindow()->ChangeScreen(m_bFullScr);
	//	�����̏����E�B���h�D�ɂ͍��m���Ă��B
	//	�i�{���́A���ׂẴE�B���h�E�Ƀu���[�h�L���X�g����K�v������j
	CAppManager::GetMyApp()->GetMyWindow()->ChangeWindowStyle();

#ifdef USE_DirectDraw
	// �v���[���̍č\�z�i�����������Ă���΁j
	CPlane::RestoreAll();
#endif

	//	DIB�̕��A����
	if (m_lpSecondary!=NULL){
		m_lpSecondary->Resize(m_nScreenXSize,m_nScreenYSize);
	}

	return 0;
}

/////// �g�p��:p) ////////////////////////////////////////////////////////////

LRESULT CDIBDraw::ChangeDisplay(bool bFullScr){
#ifdef USE_DIRECTX	// �ǉ�
	if (GetDDraw()==NULL) return 1; //	���߂������...
#endif

	BeginChangeDisplay();
		if (bFullScr) {
			TestDisplayMode(m_nScreenXSize,m_nScreenYSize,true,m_nScreenColor);
		}
		TestDisplayMode(m_nScreenXSize,m_nScreenYSize); // �E�B���h�D���[�h
	if (EndChangeDisplay()) return 2;	//	�f�B�X�v���C���[�h�ύX�Ɏ��s
	return 0;
}

LRESULT		CDIBDraw::SetDisplay(bool bFullScr,int nSizeX,int nSizeY,int nColorDepth){
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

void		CDIBDraw::GetDisplay(bool&bFullScr,int &nSizeX,int &nSizeY,int &nColorDepth){
	bFullScr		=	m_bFullScr;
	nSizeX			=	m_nScreenXSize;
	nSizeY			=	m_nScreenYSize;
	nColorDepth		=	m_nScreenColor;
}

/*
void		CDIBDraw::GetSize(int &nSizeX,int &nSizeY){ // GetDisplayMode��x,y�����������
	nSizeX			=	m_nScreenXSize;
	nSizeY			=	m_nScreenYSize;
}
*/
	
bool		CDIBDraw::IsFullScreen(void){
	return		m_bFullScr;
}

void	CDIBDraw::SetBrightness(int nBright){
	if (m_nBrightness == nBright) return ;
	m_nBrightness = nBright;
	m_bDirty = true;
}

int		CDIBDraw::GetBpp(void){
	// ���݂�Bpp�̎擾
	return CBppManager::GetBpp();
}

//////////////////////////////////////////////////////////////////////////////

void	CDIBDraw::SetOffset(int ox,int oy){
	//	�Z�J���_���̓]���I�t�Z�b�g
	m_nSecondaryOffsetX =	ox;
	m_nSecondaryOffsetY =	oy;
}

void	CDIBDraw::OnDraw(RECT* lpRect,bool bLayerCallBack){
	if (GetSecondary()==NULL) return ;
	HDC hDCS = GetSecondary()->GetDC();
	if (hDCS == NULL) return ;	// �Ȃ����D�D
	
	HDC hdc;
	if (!IsFullScreen()){
		hdc = ::GetDC(m_hWnd);
	} else {
		hdc = ::GetWindowDC(m_hWnd);
	}
	if (hdc == NULL) return ;	// ������[�O�O�G
/*

��ނ�����F('01/08/12)

> GetDC(NULL) �Ƃ������߂� WM_PAINT �����ׂẴE�C���h�E�ɑ����邽�߁A
>			^^^^^^
> GetWindowDC(m_hWnd) �Ƃ���� WM_PAINT �͑����Ȃ��c�Ƃ������Ƃ��Ǝv���܂��B

*/

	//////////////////////////////////////////////////////////
	//	Layer callback

	if ( bLayerCallBack ){
	//	Layer���Z�J���_���ɕ`�悵�Ă��炤�O�O
	m_LayerList.OnDraw(this);

	//	HDCLayer���Z�J���_���ɕ`�悵�Ă��炤�O�O
	if (!m_HDCLayerList.IsEmpty()){
		m_HDCLayerList.OnDraw(hDCS);
		Invalidate();	//	��������������ŁA�����t���O���Ă�
	}

	//	AfterLayer���Z�J���_���ɕ`�悵�Ă��炤�O�O
	m_AfterLayerList.OnDraw(this);
	}
	//////////////////////////////////////////////////////////

	//	�v���C�}��������Ă��Ȃ���΃��^�[��
	if (!m_bDirty) goto ExitDraw;

	//	�u���C�g�l�X�ɉ����āA�Z�J���_��������
	if (m_nBrightness!=256){
		GetSecondary()->FadeEffect(m_nBrightness);
	}
	InnerOnDraw(hdc,hDCS,lpRect);

ExitDraw:;
	if (!IsFullScreen()){
		::ReleaseDC(m_hWnd,hdc);
	} else {
		::ReleaseDC(m_hWnd,hdc);
	}

	m_bDirty = false;
}

void	CDIBDraw::InnerOnDraw(HDC hDst,HDC hSrc,RECT* lpRect){
	//	�ŏ�������Ă���Ȃ�Ε`�悹���Ƀ��^�[��
	if (CAppManager::GetMyApp()->GetMyWindow()->IsMinimized()) return ;

	//	Menu��Popup����
	int nMenu = 0;
	//	FullScr�ŁA���j���[����ŁA�}�E�X����ʏ�̃��j���[�̈�ɂ������������̂Ȃ��...
	if (IsFullScreen() && ::GetActiveWindow()==m_hWnd) {
	//						������Ƀ��C���E�C���h�E���A�N�e�B�u(�_�C�A���O�Ȃǂ��Ăяo����ĂȂ�)
		POINT point;
		::GetCursorPos(&point);
		if (point.y<m_nMenu) {
			if (::GetMenu(m_hWnd)!=NULL){
//				�����̃��j���[�T�C�Y�𐳊m�ɎZ�o���邽�߁A���j���[�����𒲂ׂ�(c)yaneurao'00/12/15
				nMenu = m_nMenu;
				//	Paint���Ȃ������[
//				::SetMenu(m_hWnd,GetMenu(m_hWnd));	//	���j���[�̈�̍ĕ`��
				::DrawMenuBar(m_hWnd);
			}
		}
	}


	if (m_nSecondaryOffsetX || m_nSecondaryOffsetY) {
		//	�]����I�t�Z�b�g���������Ă���̂��H
		RECT r; // source rect
		RECT r2; // distination rect
		if ( lpRect == NULL ){
			::SetRect(&r,0,nMenu,m_nScreenXSize,m_nScreenYSize);
			::SetRect(&r2,0,nMenu,m_nScreenXSize,m_nScreenYSize);	//	����T�C�Y�Ɖ��肵�ėǂ�
			
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
			
			::BitBlt(hDst,r2.left,r2.top,r.right-r.left,r.bottom-r.top,hSrc,r.left,r.top,SRCCOPY);
			
			
			RECT br;
			// fix '01/04/25 kaine
			int top;
			top = r2.top;
			if ( nMenu && top > 0 ) top = nMenu;
			::SetRect(&br,0,nMenu,m_nScreenXSize,top);					//	��̈�	
			//::SetRect(&br,0,nMenu,m_nScreenXSize,r2.top);					//	��̈�	
			::BitBlt(hDst,br.left,br.top,br.right-br.left,br.bottom-br.top,hSrc,r.left,r.top,BLACKNESS);
			::SetRect(&br,0,r2.top,r2.left,r2.bottom);					//	���̈�
			::BitBlt(hDst,br.left,br.top,br.right-br.left,br.bottom-br.top,hSrc,r.left,r.top,BLACKNESS);
			::SetRect(&br,r2.right,r2.top,m_nScreenXSize,r2.bottom);	//	�E�̈�
			::BitBlt(hDst,br.left,br.top,br.right-br.left,br.bottom-br.top,hSrc,r.left,r.top,BLACKNESS);
			::SetRect(&br,0,r2.bottom,m_nScreenXSize,m_nScreenYSize);	//	���̈�
			::BitBlt(hDst,br.left,br.top,br.right-br.left,br.bottom-br.top,hSrc,r.left,r.top,BLACKNESS);
		}else{
			// �I�t�Z�b�g�]���̕����X�V�Ȃ̂ŁA���ꂽ���w�i�͕`�悵�Ȃ�
			r = *lpRect;	// source rest;
			r2 = *lpRect;	// dest	 rest;

			r2.left += m_nSecondaryOffsetX;
			r2.top += m_nSecondaryOffsetY;

			if ( nMenu && nMenu > r2.top ) {
				r.top += - r2.top+nMenu;
				r2.top = nMenu;
			}
				
			::BitBlt(hDst,r2.left,r2.top,r.right-r.left,r.bottom-r.top,hSrc,r.left,r.top,SRCCOPY);
			
		}
		
	} else {
		if ( lpRect == NULL ){
			//	�]����I�t�Z�b�g�����́A���ʂ̓]��
			::BitBlt(hDst,0,nMenu,
				m_nScreenXSize,m_nScreenYSize-nMenu,hSrc,0,nMenu,SRCCOPY);
		}else{
			// �I�t�Z�b�g�Ȃ��A��`�L��
			RECT r = *lpRect;
			if ( nMenu > r.top ) r.top = nMenu;
			::BitBlt(hDst,r.left,r.top,r.right-r.left,r.bottom-r.top,hSrc,r.left,r.top,SRCCOPY);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//	CPlaneBase�̃����o�֐��B�S���A�Ϗ��O�O�G
LRESULT CDIBDraw::Blt(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return static_cast<CPlaneBase*>(GetSecondary())->Blt(lpSrc,x,y,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDIBDraw::BltFast(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return static_cast<CPlaneBase*>(GetSecondary())->BltFast(lpSrc,x,y,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDIBDraw::BlendBlt(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
						   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return static_cast<CPlaneBase*>(GetSecondary())->BlendBlt(lpSrc,x,y,dwDstRGBRate,dwSrcRGBRate,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDIBDraw::BlendBltFast(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
							   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return static_cast<CPlaneBase*>(GetSecondary())->BlendBltFast(lpSrc,x,y,dwDstRGBRate,dwSrcRGBRate,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDIBDraw::MosaicEffect(int d, LPRECT lpRect){
	return static_cast<CPlaneBase*>(GetSecondary())->MosaicEffect(d,lpRect);
}
LRESULT CDIBDraw::FlushEffect(LPRECT lpRect){
	return static_cast<CPlaneBase*>(GetSecondary())->FlushEffect(lpRect);
}
void	CDIBDraw::GetSize(int &x,int &y) {
	static_cast<CPlaneBase*>(GetSecondary())->GetSize(x,y);
}
LRESULT CDIBDraw::ClearRect(LPRECT lpRect){
	return static_cast<CPlaneBase*>(GetSecondary())->ClearRect(lpRect);
}
LRESULT CDIBDraw::Load(string szBitmapFileName,bool bLoadPalette){
	return static_cast<CPlaneBase*>(GetSecondary())->Load(szBitmapFileName,bLoadPalette);
}
LRESULT CDIBDraw::LoadW(string szBitmapFileName256,string szBitmapFileNameElse
			,bool bLoadPalette){
	return static_cast<CPlaneBase*>(GetSecondary())->LoadW(
		szBitmapFileName256,szBitmapFileNameElse,bLoadPalette);
}
LRESULT CDIBDraw::Release(void){
	return static_cast<CPlaneBase*>(GetSecondary())->Release();
}
LRESULT CDIBDraw::SetColorKey(int x,int y){
	return static_cast<CPlaneBase*>(GetSecondary())->SetColorKey(x,y);
}
LRESULT CDIBDraw::SetColorKey(int r,int g,int b){
	return static_cast<CPlaneBase*>(GetSecondary())->SetColorKey(r,g,b);
}

bool CDIBDraw::IsLoaded(void) const {
	//	GetSecondary()�ł́Aconst����������̂Łc
	return m_lpSecondary->IsLoaded();
}

LRESULT CDIBDraw::BltClearAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return GetSecondary()->BltClearAlpha(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDIBDraw::BltFastClearAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return GetSecondary()->BltFastClearAlpha(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDIBDraw::BlendBltFastAlpha(CPlaneBase* lpDIBSrc32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return GetSecondary()->BlendBltFastAlpha(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDIBDraw::BlendBltFastAlpha(CPlaneBase* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
							   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return GetSecondary()->BlendBltFastAlpha(lpDIBSrc32,x,y,dwDstRGBRate,dwSrcRGBRate,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDIBDraw::FadeBltAlpha(CPlaneBase* lpDIBSrc32,int x,int y,int nFadeRate){
	return ((CPlaneBase*)GetSecondary())->FadeBltAlpha(lpDIBSrc32,x,y,nFadeRate);
}

LRESULT CDIBDraw::BltClearAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return GetSecondary()->BltClearAlphaM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDIBDraw::BltFastClearAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return GetSecondary()->BltFastClearAlphaM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDIBDraw::BlendBltFastAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return GetSecondary()->BlendBltFastAlphaM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDIBDraw::BlendBltFastAlphaM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
							   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return GetSecondary()->BlendBltFastAlphaM(lpDIBSrc32,x,y,dwDstRGBRate,dwSrcRGBRate,lpSrcRect,lpDstSize,lpClipRect);
}

LRESULT CDIBDraw::Clear(DWORD dwDIB32RGB,LPRECT lpRect){
	return GetSecondary()->Clear(dwDIB32RGB,lpRect);
}

LRESULT CDIBDraw::BltNatural(CPlaneBase* lpDIBSrc32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return GetSecondary()->BltNatural(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
}
LRESULT CDIBDraw::BltNatural(CPlaneBase* lpDIBSrc32,int x,int y,int nFade,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	return GetSecondary()->BltNatural(lpDIBSrc32,x,y,nFade,lpSrcRect,lpDstSize,lpClipRect);
}

int		CDIBDraw::GetMenuHeight(void){
//	���j���[�����Ă���Ȃ�΁A���̃��j���[������Ԃ�
//	�i���Ă��Ȃ���΂O�j
	if (::GetMenu(m_hWnd)!=NULL) return m_nMenu;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT CDIBDraw::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){

	switch(uMsg){
	case WM_PAINT : {
		//	dirty rect������
		//	�t���X�N���[���Ŕ�������WM_PAINT�͂ǂ������������O�O�G
		/*
		��'00/11/28 ���b�Z�[�W�{�b�N�X�\�����Ă�Ɣ������邩�c
			�������A�E�B���h�D���X�N���[�����W��(0,0)����`�悵�Ă���̂ŁA
			BeginPaint�`EndPaint�ł͐������\������Ȃ��B
			SetWindowPos�ŁA�N���C�A���g���W(0,0)���X�N���[�����W��(0,0)��
			����悤�Ɏw�肵�Ă�����͂��邪�A��������ƍ��x�̓��j���[�o�[��
			���O�ŕ`�悵�Ȃ��Ă͂Ȃ�Ȃ��Ȃ�B�܂��ABeginPaint�`EndPaint��hWnd��
			NULL��n�����Ƃ��Ă��A�O�ʂɃE�B���h�D������̂Ńo�b�N�O���E���h��
			������X�V���Ă������B

			�����āAGetDC(NULL)���悤�ɂ��AWM_PAINT�̏������ɂ�GetDC�͎g���Ȃ��B
			GetDC�Ŏ擾�����f�o�C�X�R���e�L�X�g�ɕ`�悷���WM_PAINT���������邩��>
			WM_PAINT�̏�������WM_PAINT�����������ŁA�A�v���P�[�V������WM_PAINT��
			���������ł��Ȃ��Ȃ�I

			�����I�ȉ�����Ƃ��ẮA���j���[�E�B���h�D�����O�ŕ`�悷�邱�Ƃ�O��ɁA
			�ȉ��ɂ��� if (IsFullScreen()) return 0;�̍s���O���A
			�E�B���h�D�������ɃN���C�A���g���W(0,0)���X�N���[�����W��(0,0)��
			����悤�Ɉړ������AInnerOnDraw�ł́A�t���X�N���[�����j���[�`��̏�����
			�s�Ȃ�Ȃ��悤�ɂ��邱�ƁB���ꂪ���Ȃ�΁A�_�C�A���O�{�b�N�X�����O��
			�`�悷�邱�ƁO�O�G

			�O�҂̏ꍇ�ɓO����Ȃ�΁A�E�B���h�D�n���h���͕s�v�Ƃ������ƂɂȂ�̂ŁA
			���܂��ʓI�ł͂Ȃ����A�t���X�N���[�����́A�E�B���h�D����炸��
			�ς܂��邱�Ƃ͏o����B

			�����΂���ɓI�ȉ����i�Ƃ��ẮA�t���X�N���[�����̃��b�Z�[�W�{�b�N�X
			�ɂ́u�ړ��ł��Ȃ��v�X�^�C�����w�肵�Ă������Ƃł���B

		*/
		m_bDirty = true;	//	����|���Ă����Ȃ��ƍĕ`�悳��Ȃ�
//		if (IsFullScreen()) return 0;

		if (GetSecondary()==NULL) return 0;
		HDC hDCS = GetSecondary()->GetDC();
		if (hDCS == NULL) return 0; // �Ȃ����D�D
		PAINTSTRUCT paintstruct;
		HDC hdc;
		if(IsFullScreen()) {// �t���X�N���[���Ȃ� GetWindowDC �擾
			// ����œ���(��)
			// GetWindowDC �ł� WM_PAINT �͔��ł��Ȃ��H
			hdc = ::GetWindowDC(m_hWnd);
		} else {
			hdc = ::BeginPaint(m_hWnd,&paintstruct);
		}
		InnerOnDraw(hdc,hDCS);
		if(IsFullScreen()) {
			// ����œ���(��)
			::ReleaseDC(m_hWnd,hdc);
		} else {
			::EndPaint(m_hWnd,&paintstruct);
		}
		return 0;	//	DefWindowProc���Ăяo���ɍs���B
		//	������Ăяo���Ă����Ȃ��ƍēxWM_PAINT�������Ă���
					}

	case WM_ACTIVATEAPP: {
		UINT bActive = wParam;
		if(bActive) {
			//	���b�Z�[�W�|���v���ł͂Ȃ��A���[�J�[�X���b�h���Ń��X�g�A���ׂ�...
			//	m_bLostSurface = true;
		}
		return 0;
						 }

	} // end switch

	return 0;
}

#endif // USE_DIB32