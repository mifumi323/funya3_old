#include "stdafx.h"

#ifdef USE_DirectDraw

#include "yanePlane.h"
#include "yaneWindow.h"
#include "yaneDirectDraw.h"
#include "yaneGraphicLoader.h"
#include "yaneDIBitmap.h"
#include "yaneDIB32.h"
#include "yaneAppManager.h"
#include "yaneAppInitializer.h"

//////////////////////////////////////////////////////////////////////////////
//	�������`�F�b�N�@�\�i'00/09/09�ǉ��j
//
//		CPlane�̂����A�r�f�I��������lock����]���iCPlane�̊g�k�����u�����h�n�j�́A
//		lock�����r�f�I�������O�̗̈�ɃA�N�Z�X���Ă��Ă��A�������G���[�ɂ͂Ȃ�Ȃ�
//		�̂ŁA�������`�F�b�N�@�\���K�v�ł���B
//
//		�ȉ���VRAM_MEMORY_CHECK��define��L���ɂ���ƁA���͂ȃ������`�F�b�N���s�Ȃ��B
//		�i�������A���̕��x���Ȃ�j
//		�����������������debug���Ɏg���Ċm�F���Ă݂�Ɨǂ��B

#ifdef _DEBUG
		#define VRAM_MEMORY_CHECK
	//	������define��L���ɂ���
#endif

//////////////////////////////////////////////////////////////////////////////

//	static members..
set<CPlane*>	CPlane::m_lpPlaneList;

//////////////////////////////////////////////////////

CPlane::CPlane(void){
	m_nSizeX	=	0;
	m_nSizeY	=	0;
	m_lpSurface	=	NULL;
	m_lpPalette	=	NULL;
	m_nSurfaceRef=	0;

	ResetColorKey();
	m_hDC		=	NULL;
	m_bUseSystemMemory	= false;
	m_bOwnerDraw=	false;
	m_bBitmap	=	false;
	m_bHybrid	=	false;	//	���ꂪtrue�ł����Surface��duplicate�͂��Ȃ�
	
	m_FillColor	=	0;
	m_dwFillColor=	0;
	
	//	�{���A���̕����A���X���b�h����̔r�����䂵�Ȃ��Ƃ����Ȃ���
	//	�}���`�X���b�h��Ή��Ƃ������Ƃ�...
	m_lpPlaneList.insert(this);
}

CPlane::~CPlane(){
	m_lpPlaneList.erase(this);
	Release();
}

void	CPlane::ReleaseDDSurface(void){
	//	�Q�ƃJ�E���g���̃����[�X
	if (m_lpSurface!=NULL || m_lpPalette!=NULL) {
		WARNING(m_nSurfaceRef==0,"CPlane::ReleaseDDSurface�ŎQ�ƃJ�E���g�ُ�")
		if (--m_nSurfaceRef==0){
			RELEASE_SAFE(m_lpSurface);
			RELEASE_SAFE(m_lpPalette);
		} else {
			//	���L���Ă���T�[�t�F�[�X��T���āA���̎Q�ƃJ�E���g������������
			for (set<CPlane*>::iterator it=m_lpPlaneList.begin();it!=m_lpPlaneList.end();it++){
				if ((*it!=this) && ((*it)->m_lpSurface == m_lpSurface)) {
					(*it)->m_nSurfaceRef--;
				}
			}
			//	���̃v���[�����܂��g���Ă���̂ŉ���͂��Ȃ�
			m_lpSurface = NULL;
			m_lpPalette = NULL;
		}
	}
	m_nSurfaceRef = 0;
}

LRESULT	CPlane::Release(void){
	ReleaseDDSurface();
	m_szBitmapFile.erase();
	m_szBitmapFile2.erase();
	m_nSizeX	=	0;
	m_nSizeY	=	0;
	m_bOwnerDraw=	false;	//	�����Ŗ߂��Ă����΂n�j
	m_bBitmap	=	false;	//	�����Ŗ߂��Ă����΂n�j
	return 0;
}

void	CPlane::GetSize(int &x,int &y){
	x = m_nSizeX;
	y = m_nSizeY;
}

//	�T�[�t�F�[�X�̃��X�g�ɑ΂��镜�A����
LRESULT	CPlane::Restore(void){
	LRESULT lr = 0;
	if (m_lpSurface!=NULL){
		if ((!m_lpSurface->IsLost()) && (!m_bUseSystemMemory)) return 0;	//	Lost���ĂȂ���[
		//	�˃V�X�e����������̃T�[�t�F�[�X�́A���X�g���Ȃ��O�O�G
		//	�̂ŁA���X�g���Ă��Ȃ��Ƃ��A���X�g�����Ƃ��Ĉ����K�v������
		//	���ۂ̂Ƃ���A�r�f�I�������Ɋm�ۂ������肪�A�V�X�e����������
		//	�m�ۂ���Ă��āA���X�g���Ȃ����Ă��Ƃ����肤��̂����D�D
		//	���̂ւ�͖����B���傠��Ȃ�CFastPlane�g���Ă���O�O�G

		//	�r�b�g�}�b�v�t�@�C���Ȃ�΂���𕜌�����
		if (m_bBitmap){
			if (m_bBitmapW){
				if (GetBpp()==8) {
					lr = InnerLoad(m_szBitmapFile,m_bLoadPalette);
				} else {
					lr = InnerLoad(m_szBitmapFile2,m_bLoadPalette);
				}
			} else {
				lr = InnerLoad(m_szBitmapFile,m_bLoadPalette);
			}
		}
		//	���݂̉�ʃ��[�h�ɍ��킹��FillColor��ݒ肵�Ȃ���
		SetFillColor(m_FillColor);
		//	�I�[�i�[�h���[�Ȃ�΁A����𕜌�����
		if (m_bOwnerDraw) {
			lr |= OnDraw();	//	�Ϗ�����
		}
	}
	return lr;
}

LRESULT CPlane::RestoreAll(void){ // �S�v���[���̃����[�h
	for (set<CPlane*>::iterator it=m_lpPlaneList.begin();it!=m_lpPlaneList.end();it++){
		//	�p���b�g�̃��A���C�Y���s���čs�Ȃ�
		if ((*it)->m_bLoadPalette) {
			(*it)->Restore();	//	�������p���b�g�̃��A���C�Y���s�Ȃ�
		}
	}
	
	//	ToDo : �����Ńp���b�g�̃t���b�V�����s�Ȃ�Ȃ��Ă͂Ȃ�Ȃ�

	for (it=m_lpPlaneList.begin();it!=m_lpPlaneList.end();it++){
		(*it)->Restore();
	}
	//	�Ƃ肠������ʕ\�����Ȃ��Ƃ����Ȃ��O�O
	return 0;
}

LRESULT	CPlane::OnDraw(void){
	return 0;
}

void	CPlane::ResetColorKey(void){
	m_bUsePosColorKey	= true;
	m_nCX = m_nCY = 0;
}

LRESULT CPlane::Load(string szBitmapFileName,bool bLoadPalette){
	ResetColorKey();
	m_bBitmapW = false;
	m_szBitmapFile = szBitmapFileName;
	m_bLoadPalette = bLoadPalette;
	// ���Ƃ�Restore�ł���悤�Ƀt�@�C�������i�[���Ă����B
	return InnerLoad(szBitmapFileName,bLoadPalette);
}

LRESULT CPlane::LoadW(string szBitmapFileName256,string szBitmapFileNameElse
	,bool bLoadPalette){
	ResetColorKey();
	m_bBitmapW = true;
	m_szBitmapFile = szBitmapFileName256;
	m_szBitmapFile2 = szBitmapFileNameElse;
	m_bLoadPalette = bLoadPalette;
	// ���Ƃ�Restore�ł���悤�Ƀt�@�C�������i�[���Ă����B
	if (GetBpp()==8) {
		return InnerLoad(szBitmapFileName256,bLoadPalette);
	} else {
		return InnerLoad(szBitmapFileNameElse,bLoadPalette);
	}
}

//	�t�@�C������Ԃ�
string	CPlane::GetFileName(void) const{
	if (m_bBitmapW && !(GetBpp()==8)) {
		return m_szBitmapFile2;
	}
	return m_szBitmapFile;
}

bool CPlane::CheckDuplicate(string szFileName){
	//	���̃T�[�t�F�[�X���畡���\�����ׁA�����\�ł���΍s�Ȃ�
	if (m_bHybrid) return false;	//	�n�C�u���b�h�v���[���Ȃ�Ε����͂��Ȃ�

	return false;

	LPDIRECTDRAW lpDraw = CAppManager::GetMyDirectDraw()->GetDDraw();
	if (lpDraw==NULL) return false;	//	�Ȃ񂶂Ⴛ���O�O

	ReleaseDDSurface();
//	RELEASE_SAFE(m_lpSurface);	//	�����̃T�[�t�F�[�X������Ă���..

	for (set<CPlane*>::iterator it=m_lpPlaneList.begin();it!=m_lpPlaneList.end();it++){
		if ((*it!=this) && (*it)->IsLoaded()
			&& ((*it)->GetFileName() == szFileName) && (!(*it)->m_bHybrid)){
//			if (lpDraw->DuplicateSurface((*it)->m_lpSurface,&m_lpSurface)==DD_OK){
				//	�T�[�t�F�[�X��񓙂��ׂăR�s�[����B
				m_nSizeX			= (*it)->m_nSizeX;
				m_nSizeY			= (*it)->m_nSizeY;

				//	�������̃p�����[�^���Z�b�g����
				m_bBitmap	=	true;
				SetColorKey();	//	�f���v�������̃J���[�L�[����ׂ����ƂɂȂ邪�D�D

				//	���O�ŎQ�ƃJ�E���g�𒲂ׂ�
				m_lpSurface			= (*it)->m_lpSurface;
				m_lpPalette			= (*it)->m_lpPalette;
				m_nSurfaceRef		= (*it)->m_nSurfaceRef;

				//	���L���Ă���T�[�t�F�[�X��T���āA���̎Q�ƃJ�E���g�������グ��(�������܂�)
				for (set<CPlane*>::iterator it=m_lpPlaneList.begin();it!=m_lpPlaneList.end();it++){
					if ((*it)->m_lpSurface == m_lpSurface) {
						(*it)->m_nSurfaceRef++;
					}
				}

				return true;	//	duplicate�I���I
//			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////

void	CPlane::ClearMSB(void){

	/*
		�����ׂ����ƂɁA�A�C�I�[�f�[�^��GA-SV4�V���[�Y�ł́A
		32bpp���[�h�ɂ����āAClear�Ŋ��S�Ƀ������̈�͂O�ɂȂ��Ă���̂ɁA
		Render�ŃS�~��MSB�ɏ������܂��B���̉摜�ǂݍ��݂́AHDC�Ԑڂ�
		��������ł���킯�ŁAHDC�̍\���̂ɉR���܂܂�Ă���Ƃ������ƂɂȂ�B
		�����ŉ摜�ǂݍ��݌�AMSB���N���A���邪�AHDC���擾���ĕ`�悷�邲�Ƃ�
		�R�����グ�Ă���Ƃ������Ƃ��Ӗ�����B����́A�Ђǂ��ł��B
		����ȏ�́A�r�f�I�J�[�h���̃h���C�o�̃o�O���ƌ����܂��B

													��˂��炨 '00/10/04
	*/

	if (GetBpp()==32){
		DDSURFACEDESC dddesc;
		ZERO(dddesc);
		dddesc.dwSize = sizeof(dddesc);
		LRESULT hres;
		while ((hres = GetSurface()->Lock(NULL, &dddesc, 0, NULL)) == DDERR_WASSTILLDRAWING)
		;
		if (hres !=DD_OK){
			Err.Out("CPlane::ClearMSB��Surface��Lock�Ɏ��s");
		}
		if ((dddesc.ddpfPixelFormat.dwRGBBitCount)==32) {
			LONG lPitch	 = dddesc.lPitch;
			DWORD RMask, GMask, BMask,RGBMask;
			RMask = dddesc.ddpfPixelFormat.dwRBitMask;
			GMask = dddesc.ddpfPixelFormat.dwGBitMask;
			BMask = dddesc.ddpfPixelFormat.dwBBitMask;
			RGBMask = RMask | GMask | BMask;
			//	RGBMask�ȊO��0�ɂ���B

			for(int y=0;y<dddesc.dwHeight;y++){
				DWORD *p	= (DWORD*)((BYTE*)dddesc.lpSurface + dddesc.lPitch * y);
				for(int x=0;x<dddesc.dwWidth;x++){
					*p = *p & RGBMask;
					p++;
				}
			}
			GetSurface()->Unlock(NULL);
		}
	}
}

//	�r�b�g�}�b�v�̓����I�ȃ��[�h�B�i�[�t�@�C�����ɂ͉e�����Ȃ�
LRESULT	CPlane::InnerLoad(string szFileName,bool bLoadPalette){
//	m_bOwnerDraw=	false;
	m_bBitmap	=	false;

	//	Duplicate�ōςނ��H
	if (CheckDuplicate(szFileName)) return 0;

	CFile file;
	if (file.Read(szFileName)) return 1;
	
	auto_ptrEx<CGraphicLoader> gl(CGraphicLoader::GetPrototypeFactory()->CreateInstance());
	if (gl->LoadPicture(file)) return 2;

	LONG sx,sy;
	if (gl->GetSize(sx,sy)) return 3;
	if (InnerCreateSurface(sx,sy)) return 4;

	HDC hdc = GetDC();
	if (hdc == NULL) return 5;
	if (gl->Render(hdc)) { ReleaseDC(); return 6; }
	ReleaseDC();
	if (gl->ReleasePicture()) return 7;

	ClearMSB();

	SetColorKey();	//	���A�D�D�D

	m_bBitmap	=	true;
	return 0;
}

HDC CPlane::GetDC(void){
	if (m_hDC!=NULL) {
		Err.Out("CPlane::EndPaint���Ăяo����Ă��Ȃ��̂�BeginPaint���Ăяo���ꂽ");
		return NULL;
	}
	if (m_lpSurface==NULL) return NULL;
	if (m_lpSurface->GetDC(&m_hDC)!=DD_OK) return NULL;
	return m_hDC;
}

void CPlane::ReleaseDC(void){
	if (m_hDC==NULL) {
		Err.Out("CPlane::BeginPaint���Ăяo����Ă��Ȃ��̂�EndPaint���Ăяo���ꂽ");
		return ;
	}
	if (m_lpSurface==NULL) return ;
	if (m_lpSurface->ReleaseDC(m_hDC)!=DD_OK) return ;
	m_hDC = NULL;
}

LRESULT	CPlane::Save(LPSTR szFileName,LPRECT lpRect){

	HDC hdc = GetDC();
	if (hdc == NULL) return 1;

	RECT rc;
	if (lpRect==NULL) {
		::SetRect(&rc,0,0,m_nSizeX,m_nSizeY);
		lpRect = &rc;
	}

	CDIBitmap dib;
	if (dib.Load(hdc,lpRect)) { ReleaseDC(); return 2;}
	ReleaseDC();
	if (dib.Save(szFileName)) { return 3;}

	return 0;
}

LRESULT CPlane::InnerCreateSurface(int sx,int sy){
//	�T�C�Y�w��Ńv���[���쐬
//	RELEASE_SAFE(m_lpSurface);
	ReleaseDDSurface();

	DDSURFACEDESC ddsd;
	ZERO(ddsd);
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;

	//	�����I�ɃV�X�e�����������g���I�v�V����
	if (m_bUseSystemMemory) {
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	} else {
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	}

	// �T�C�Y��ۑ����Ă���
	m_nSizeX = ddsd.dwWidth	 = sx;
	m_nSizeY = ddsd.dwHeight = sy;

	LPDIRECTDRAW lpDraw = CAppManager::GetMyDirectDraw()->GetDDraw();
	if (lpDraw==NULL) return 1;
	if (lpDraw->CreateSurface(&ddsd,&m_lpSurface,NULL)!=DD_OK){
		Err.Out("CPlane::CreateSurface��CreateSurface�Ɏ��s");
		return 2; // ������[
	}

	m_nSurfaceRef = 1;	//	�Q�ƃJ�E���g�̐ݒ�O�O

	Clear();	//	�O�̂��߃N���A���Ă���(�ŏ�ʂ�0�ɂ��邽��)
	return 0;
}

LRESULT CPlane::CreateSurface(int sx,int sy,bool bYGA){
	//	���ʂ�CreateSurface����ƁA����͊ԈႢ�Ȃ��I�[�i�[�h���[�v���[���ł���
	m_bOwnerDraw	= false;
	m_bBitmap		= false;
	ResetColorKey();
	if (bYGA) return -1;	//	YGA�T�[�t�F�[�X�Ȃ񂼌������͖̂���
	LRESULT lr = InnerCreateSurface(sx,sy);
	if (lr) return lr;
	SetColorKey();
	m_bOwnerDraw	= true;

	//	CreateSurface���Ă�񂾂���AFillColor�̓��Z�b�g���ׂ� '00/09/09
	SetFillColor(0);

	return 0;
}

//	�v���C�}���T�[�t�F�[�X�̐���
LRESULT	CPlane::CreatePrimary(bool& bUseFlip,int nSx,int nSy){
	Release();
	ResetColorKey();
	LPDIRECTDRAW lpDraw = CAppManager::GetMyDirectDraw()->GetDDraw();
	if (lpDraw==NULL) return 1;
	if (!CAppManager::GetMyDirectDraw()->IsFullScreen()) bUseFlip = false;

sur_retry: ;
	DDSURFACEDESC ddsd;
	ZERO(ddsd);
	ddsd.dwSize = sizeof(ddsd);

	if (bUseFlip) {
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE
			| DDSCAPS_FLIP
			| DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;
	} else {
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	}
	if (lpDraw->CreateSurface(&ddsd,&m_lpSurface,NULL)!=DD_OK){
		Err.Out("CPlane::CreatePrimary�Ɏ��s");
		if (bUseFlip) {
			bUseFlip=false;
			// Flipping Surface�̓r�f�I��������ɔz�u����K�v�����邽�߁A
			// Create�Ƀ~�X�����Ƃ��l������
			goto sur_retry;
		}
		return 1; // ������[
	}
	//	�����ŃR�s�[���Ă���
	if (nSx && nSy) {
		m_nSizeX = nSx; m_nSizeY = nSy;	//	�T�C�Y�͂�����̗p
	} else {
		CAppManager::GetMyDirectDraw()->GetSize(m_nSizeX,m_nSizeY);
	}

	m_nSurfaceRef = 1;		//	�Q�ƃJ�E���g�����Ă����Ȃ��Ƃ��܂��������Ȃ�
	m_bOwnerDraw = true;	//	�����On�ɂ��Ȃ���Restore����Ă��܂�
	return 0;
}

//	�Z�J���_���T�[�t�F�[�X�̐���
LRESULT CPlane::CreateSecondary(CPlane*lpPrimary,bool& bUseFlip){
	Release();
	ResetColorKey();

	LPDIRECTDRAW lpDraw = CAppManager::GetMyDirectDraw()->GetDDraw();
	if (lpDraw==NULL) return 1;

sur_retry: ;
	if (bUseFlip) {
		// flip���g���ȏ�A�V�X�e���������Ƀo�b�t�@���m�ۂ���̂͂܂����D�D�D
		DDSCAPS ddscaps;		
		ZERO(ddscaps);	//	����Ȃ����ǈꉞ��
		if (m_bUseSystemMemory) {
			ddscaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_SYSTEMMEMORY;
		} else {
			ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		}
		if (lpPrimary->GetSurface()->GetAttachedSurface(&ddscaps,&m_lpSurface)!=DD_OK){
			Err.Out("CPlane::CreateSecondary��GetAttachedSurface�Ɏ��s");
			// �ق₩�猾��񂱂�����Ȃ��I
			bUseFlip = false;
			goto sur_retry;
		}
		lpPrimary->GetSize(m_nSizeX,m_nSizeY);
	} else {
		DDSURFACEDESC ddsd;
		ZERO(ddsd);
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		if (m_bUseSystemMemory) {		
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		} else {
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		}
		lpPrimary->GetSize(m_nSizeX,m_nSizeY);
		ddsd.dwWidth  = m_nSizeX;
		ddsd.dwHeight = m_nSizeY;
		if (lpDraw->CreateSurface(&ddsd,&m_lpSurface,NULL)!=DD_OK){
			Err.Out("CPlane::CreateSecondary�Ɏ��s");
			return 1; // ������[
		}
	}

	//	�ꉞ�N���A���Ƃ���...
	Clear();

	m_nSurfaceRef = 1;		//	�Q�ƃJ�E���g�����Ă����Ȃ��Ƃ��܂��������Ȃ�
	m_bOwnerDraw = true;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

LPDIRECTDRAWSURFACE CPlane::GetSurface(void){
	return m_lpSurface;
}

LPDIRECTDRAWPALETTE CPlane::GetPalette(void){
	return m_lpPalette;
}

LRESULT CPlane::SetSystemMemoryUse(bool bEnable){
	m_bUseSystemMemory = bEnable;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CPlane::SetColorKey(void){
	if (m_bUsePosColorKey) {	// �ʒu�w��^���߃L�[
		if (SetColorKey(m_nCX,m_nCY)) {
			Err.Out("CPlane::�ʒu�w��^���߃L�[�̐ݒ�Ɏ��s");
			// �G���[�Ƃ͂���
		}
	} else {	// �F�w��^���߃L�[
	// �f�B�t�H���g�ł́A�Ƃ肠����rgb = 0�𓧉߃L�[�Ƃ��Đݒ�
	// �i�s�v�̂Ƃ���Blt���ɓ��߃L�[�𖳎�������̂��g���Ηǂ��j
		if (SetColorKey(m_ColorKey)) {
			Err.Out("CPlane::�F�w��^���߃L�[�̐ݒ�Ɏ��s");
			// �G���[�Ƃ͂���
		}
	}
	return 0;
}

LRESULT CPlane::SetColorKey(int r,int g,int b){
	return SetColorKey(RGB(r,g,b));
}

LRESULT CPlane::SetColorKey(COLORREF rgb)
{
	if (m_lpSurface==NULL) return -1;

	DDCOLORKEY			ddck;

	ddck.dwColorSpaceLowValue  = DDColorMatch(m_lpSurface,rgb); // �T�[�t�F�[�X��̂ǂ̓_�����ׂ�
	ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;

	m_bUsePosColorKey = false;
	m_ColorKey = rgb;	// ����ۑ����Ƃ���ƕ��A�ł��ւ�:p

	return m_lpSurface->SetColorKey(DDCKEY_SRCBLT, &ddck);
}

DWORD CPlane::DDColorMatch(LPDIRECTDRAWSURFACE pdds, COLORREF rgb)
{
	// ddutil.cpp���Q�l�ɂ��Ă͂��邪�ADirectDrawPalette�́AGDI���o�R���Ȃ�����
	// GDI��GetPixel�ŉR���Ԃ��Ă��邱�Ƃ�����B
	HDC hdc;
	DWORD dw = CLR_INVALID;
	DDSURFACEDESC ddsd;
	LRESULT hres;
	DWORD dwRGB;

	// �Q�T�U�F���[�h�̂��߂ɁA���炩����LOCK���Ēl��ۑ�����
	ddsd.dwSize = sizeof(ddsd);
	while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
	;

	if (hres == DD_OK) {
		dwRGB = *(DWORD *)ddsd.lpSurface;
		pdds->Unlock(NULL);
	}

	//	GDI��SetPixel���āA�����𒼂�Lock���ēǂݍ��ނ��ƂŃp���b�g�̊��蓖�ď󋵂�m��

	if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK){
		::SetPixel(hdc, 0, 0, rgb);				// set our value
		pdds->ReleaseDC(hdc);
	}

	ddsd.dwSize = sizeof(ddsd);
	while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
	;

	if (hres == DD_OK) {
		dw	= *(DWORD *)ddsd.lpSurface;						// get DWORD
		if(ddsd.ddpfPixelFormat.dwRGBBitCount < 32)
			dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount)-1;	// mask it to bpp
		*(DWORD *)ddsd.lpSurface = dwRGB;

		//	����������ł́A32bpp�ɂ�����ŏ�ʃo�C�g���s��ł���{�[�h�Ȃǂł͔����F��
		//	���\��������...(�r�f�I�J�[�h��bug)	fixed by ��˂��炨 ('00/08/02)

		if (ddsd.ddpfPixelFormat.dwRGBBitCount != 8){
			//	������256�F���[�h�ł́ARGB�}�X�N�͉R�ɂȂ�̂��O�O;
			//		fixed by JesterSera ('01/02/22)
			DWORD RMask = ddsd.ddpfPixelFormat.dwRBitMask;
			DWORD GMask = ddsd.ddpfPixelFormat.dwGBitMask;
			DWORD BMask = ddsd.ddpfPixelFormat.dwBBitMask;
			DWORD RGBMask = RMask | GMask | BMask;
			dw &= RGBMask;
		}

		pdds->Unlock(NULL);
	} else {
		Err.Out("DDColorMatch::Surface�̃��b�N�Ɏ��s...");
	}

	return dw;
}

LRESULT CPlane::SetColorKey(int x,int y){	// (x,y)�̓_�𓧉߃L�[�ɐݒ肷��
	if (m_lpSurface==NULL) return -1;

	DDCOLORKEY			ddck;

	ddck.dwColorSpaceLowValue  = DDGetPixel(m_lpSurface,x,y);
	ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;

	m_bUsePosColorKey = true;
	m_nCX = x;	// ����ۑ����Ƃ���ƕ��A�ł��ւ�
	m_nCY = y;

	return m_lpSurface->SetColorKey(DDCKEY_SRCBLT, &ddck);
}

DWORD CPlane::DDGetPixel(LPDIRECTDRAWSURFACE pdds,int x,int y){ // ����̓_�̐F�𒲂ׂ�
	// ����Surface��Lock
	DDSURFACEDESC dddesc;
	ZERO(dddesc);
	dddesc.dwSize = sizeof(dddesc);
	if (pdds->Lock(NULL,&dddesc,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::DDGetPixel��Surface��Lock�Ɏ��s");
		return CLR_INVALID;
	}

	//	�O�������p���̂Ŕ͈͊O�`�F�b�N���܂��K�v
	if (x<0 || x>=dddesc.dwWidth || y<0 || y>=dddesc.dwHeight){
		pdds->Unlock(NULL);
		return CLR_INVALID;
	}

	int bpp;
	bpp = dddesc.ddpfPixelFormat.dwRGBBitCount;

	void *p = dddesc.lpSurface;
	LONG lPitch	 = dddesc.lPitch;

	DWORD dw;
	switch (bpp) {
	case 8:
		dw = *((BYTE*)p + x + y * lPitch);
		break;
	case 16:
		dw = *(WORD*)((BYTE*)p + x*2 + y * lPitch);
		break;
	case 24:
		//	���ꂪ�������ی�ᔽ�ɂȂ肤��
		//	dw = *(DWORD*)((BYTE*)p + x*3 + y * lPitch) & 0xffffff;
		{
			BYTE* lp = (BYTE*)p + x*3 + y * lPitch;
			dw = (DWORD)*lp + (((DWORD)lp[1])<<8) + (((DWORD)lp[2])<<16);
		}
		break;
	case 32:
		dw = *(DWORD*)((BYTE*)p + x*4 + y * lPitch);
		break;
	default:
		dw = CLR_INVALID; // unsupported!!
	}

	//	����������ł́A32bpp�ɂ�����ŏ�ʃo�C�g���s��ł���{�[�h�Ȃǂł͔����F��
	//	���\��������...(�r�f�I�J�[�h��bug)	fixed by ��˂��炨 ('00/10/02)
	if (bpp != 8){
		//	256�F���[�h�ł́ARGB�}�X�N�͉R�ɂȂ�̂��O�O;
		//		fixed by JesterSera ('01/01/19)
		DWORD RMask = dddesc.ddpfPixelFormat.dwRBitMask;
		DWORD GMask = dddesc.ddpfPixelFormat.dwGBitMask;
		DWORD BMask = dddesc.ddpfPixelFormat.dwBBitMask;
		DWORD RGBMask = RMask | GMask | BMask;
		if (dw!=CLR_INVALID){
			dw &= RGBMask;
		}
	}

	pdds->Unlock(NULL);

	return dw;
}

LRESULT		CPlane::SetFillColor(COLORREF c){
	if (m_lpSurface==NULL) return -1;
	m_FillColor = c;
	m_dwFillColor = DDColorMatch(m_lpSurface,c);
	if (m_dwFillColor == CLR_INVALID) return 1;
	return 0;
}

DWORD		CPlane::GetFillColor(void){
	return m_dwFillColor;
}

LRESULT		CPlane::Clear(LPRECT lpRect){
	if (m_lpSurface==NULL) return -1;
	DDBLTFX fx;
	ZERO(fx);
	fx.dwSize = sizeof(fx);
	fx.dwFillColor = m_dwFillColor;
	return m_lpSurface->Blt(lpRect,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
}

//////////////////////////////////////////////////////////////////////////////

// �f�B�X�v���C�̐F���𒲂ׂ�̂�GetDisplayMode�͎g���Ă͂����Ȃ�
int		CPlane::GetBpp(void){
	return CBppManager::GetBpp();
}

void	CPlane::InnerGetBpp(void) {
	CBppManager::Reset();
}

void	CPlane::SwapPlane(CPlane*lpPlane){
	//	�ꎞ�I�ɓ���ւ���̂��O�O
	Swap(m_nSizeX,lpPlane->m_nSizeX);
	Swap(m_nSizeY,lpPlane->m_nSizeY);
	Swap(m_lpSurface,lpPlane->m_lpSurface);
}

//////////////////////////////////////////////////////////////////////////////
//	CPlane <--> CDIB32
#ifdef USE_DIB32

LRESULT		CPlane::Blt(CDIB32*lpSrc,int x,int y,LPRECT lpSrcRect,LPRECT lpClipRect){
	if (m_lpSurface==NULL) return -1;
	return lpSrc->BltToPlane(this,x,y,lpSrcRect,lpClipRect);
}

LRESULT		CPlane::BltTo(CDIB32*lpDst,int x,int y,LPRECT lpSrcRect,LPRECT lpClipRect){
	if (m_lpSurface==NULL) return -1;
	return lpDst->BltFromPlane(this,x,y,lpSrcRect,lpClipRect);
}

#endif
//////////////////////////////////////////////////////////////////////////////
//	�v���[���ԓ]���n�̎���
//////////////////////////////////////////////////////////////////////////////

//	general clipping algorithms by yaneurao(M.Isozaki)
// ����������`��Blt����̂�Clipper�Ȃ�Ă���炭���[�I
// �蓮��Clip�����ف[������ۂǑ����I(BltFast�g���邵)
// ���O�œ]������ق���DirectDrawClipper������ۂǑ���
// �]����Rect�̎Z�o
#define DRAW_CLIPPER \
	RECT clip;									\
	if (lpClipDstRect==NULL){					\
		::SetRect(&clip,0,0,m_nSizeX,m_nSizeY);	\
	} else {									\
		clip = *lpClipDstRect;					\
	}											\
	RECT sr;									\
	if (lpSrcRect==NULL){						\
		::SetRect(&sr,0,0,lpSrc->m_nSizeX,lpSrc->m_nSizeY);	\
	} else {									\
		sr = *lpSrcRect;						\
	}											\
	LPDIRECTDRAW lpDraw = CAppManager::GetMyDirectDraw()->GetDDraw();	\
	if (lpDraw==NULL) return 1;									\
	if (m_lpSurface==NULL) return 2;							\
	LPDIRECTDRAWSURFACE lpSrcSurface = lpSrc->GetSurface();		\
	if (lpSrcSurface==NULL) return 3;							\
	int x2,y2;													\
	x2 = x + sr.right - sr.left; /* x + Width  */				\
	y2 = y + sr.bottom - sr.top; /* y + Height */				\
	if (x2<clip.left || y2<clip.top								\
		|| x>=clip.right || y>=clip.bottom) return 0;  /* ��ʊO */ \
	int t;														\
	t = clip.left-x;										\
	if (t>0)	{ sr.left	+= t;	x = clip.left;	}		\
	t = clip.top -y;										\
	if (t>0)	{ sr.top	+= t;	y = clip.top;	}		\
	t = x2-clip.right;										\
	if (t>0)	{ sr.right	-= t;	x2= clip.right;	}		\
	t = y2-clip.bottom;										\
	if (t>0)	{ sr.bottom	-= t;	y2= clip.bottom;}		\
	if (sr.right<=sr.left || sr.bottom<=sr.top) return 0;	// invalid rect
//	�����܂�
////////////////////////////////////////////////////////////////////

LRESULT		CPlane::Blt(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect,LPRECT lpClipDstRect){
// ��`�̓]���B�������A���̓]������`�͓_(lpSrcRect->right,lpSrcRect->bottom)�͊܂܂Ȃ����Ƃɒ��ӁI
	if (m_lpSurface==NULL) return -1;
	
	DRAW_CLIPPER;

	WARNING(sr.left<0 || sr.right>lpSrc->m_nSizeX || sr.top<0 || sr.bottom>lpSrc->m_nSizeY,
		"CPlane::Blt�̓]�����]������`�O����s�Ȃ��Ă��܂�");
	return m_lpSurface->BltFast(x,y,lpSrcSurface,&sr,DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
}

LRESULT		CPlane::BltFast(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect,LPRECT lpClipDstRect){
// ��`�̓]���B�������A���̓]������`�͓_(lpSrcRect->right,lpSrcRect->bottom)�͊܂܂Ȃ����Ƃɒ��ӁI
	if (m_lpSurface==NULL) return -1;

	DRAW_CLIPPER;

	WARNING(sr.left<0 || sr.right>lpSrc->m_nSizeX || sr.top<0 || sr.bottom>lpSrc->m_nSizeY,
		"CPlane::BltFast�̓]�����]������`�O����s�Ȃ��Ă��܂�");
	return m_lpSurface->BltFast(x,y,lpSrcSurface,&sr,DDBLTFAST_WAIT);
}

LRESULT		CPlane::BlendBlt(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect,LPRECT lpClipDstRect){
	if (m_lpSurface==NULL) return -1;

	DRAW_CLIPPER;

	WARNING(sr.left<0 || sr.right>lpSrc->m_nSizeX || sr.top<0 || sr.bottom>lpSrc->m_nSizeY,
		"CPlane::BlendBlt�̓]�����]������`�O����s�Ȃ��Ă��܂�");

	//	�u�����h�䗦�̏�E�`�F�b�N
	WARNING(ar+br>256,"CPlane::BlendBlt��ar+br����E���z���Ă��܂�");
	WARNING(ag+bg>256,"CPlane::BlendBlt��ag+bg����E���z���Ă��܂�");
	WARNING(ab+bb>256,"CPlane::BlendBlt��ab+bb����E���z���Ă��܂�");

	DWORD	dwColorKey;
	DDCOLORKEY	ddck;
	if (lpSrcSurface->GetColorKey(DDCKEY_SRCBLT, &ddck)==DD_OK) {;	// ���߃L�[�𓾂�
		dwColorKey =	ddck.dwColorSpaceLowValue;
	} else {
		dwColorKey = CLR_INVALID; // ����������Ȃ���
	}

	// DstSurface��lock
	DDSURFACEDESC dddesc;
	ZERO(dddesc); // �ꉞ��
	dddesc.dwSize = sizeof(dddesc);
	if (m_lpSurface->Lock(NULL,&dddesc,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::BlendBlt��Surface��Lock�Ɏ��s");
		return 2;
	}

	// SrcSurface��lock
	DDSURFACEDESC dddesc2;
	ZERO(dddesc2);
	dddesc2.dwSize = sizeof(dddesc2);
	if (lpSrcSurface->Lock(NULL,&dddesc2,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::BlendBlt��Surface��Lock�Ɏ��s");
		return 3;
	}

	// 1���X�^���̑���
	LONG lPitch	 = dddesc.lPitch;
	LONG lPitch2 = dddesc2.lPitch;

	switch (dddesc.ddpfPixelFormat.dwRGBBitCount) {
	case 4:	//	16�F��Ή�
	case 8:	//	256�F��Ή�
		break;

	case 16: { // 65536�F���[�h
		WORD RMask, GMask, BMask,RGBMask;

		RMask = (WORD)dddesc.ddpfPixelFormat.dwRBitMask;
		GMask = (WORD)dddesc.ddpfPixelFormat.dwGBitMask;
		BMask = (WORD)dddesc.ddpfPixelFormat.dwBBitMask;
		RGBMask = RMask | GMask | BMask;

		WORD *p	 = (WORD*)dddesc.lpSurface;
		WORD *p2 = (WORD*)dddesc2.lpSurface;

		p  = (WORD*)((BYTE*)p +(lPitch *y));	// �Z�J���_���̓]���J�ny���W
		p2 = (WORD*)((BYTE*)p2+(lPitch2*sr.top)) + sr.left - x;	// �������

		for(int y3=y;y3<y2;y3++) {
			for (int x3=x;x3<x2;x3++) {
				WORD pixel, px,px2;

#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(&p2[x3]) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/2;
		if (dwByte<0 || x>=lpSrc->m_nSizeX || y>=lpSrc->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBlt�̈ᔽ");
			break;
		}
	}
#endif
				px2= p2[x3];
				if ((px2&RGBMask)!=(dwColorKey&RGBMask)
					|| (dwColorKey==CLR_INVALID)){	// �]�����J���[�L�[

#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(&p[x3]) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/2;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBlt�̈ᔽ");
			break;
		}
	}
#endif
					px = p [x3];
					pixel  = ((WORD)((((px2&RMask)*ar)+((px&RMask)*br))>>8)&RMask);
					pixel |= ((WORD)((((px2&GMask)*ag)+((px&GMask)*bg))>>8)&GMask);
					pixel |= ((WORD)((((px2&BMask)*ab)+((px&BMask)*bb))>>8)&BMask);
					p[x3] = pixel;
				}
			}
			p  = (WORD*)((BYTE*)p +lPitch ); // �P���X�^���̑���
			p2 = (WORD*)((BYTE*)p2+lPitch2);
			// BYTE�ɃL���X�g���Ă����Ȃ��ƌv�Z�ԈႤ:p
		}
	} break;

	case 24: {
		BYTE *p	 = (BYTE*)dddesc.lpSurface;
		BYTE *p2 = (BYTE*)dddesc2.lpSurface;

		p  = (BYTE*)p +(lPitch*y);	// �Z�J���_���̓]���J�ny���W
		p2 = (BYTE*)p2+(lPitch2*sr.top) + (sr.left - x)*3; // �������

		int ox=x*3,ox2=x2*3;

		BYTE r,g,b;
		r = dwColorKey & 0xff;
		g = (dwColorKey >> 8) & 0xff;
		b = (dwColorKey >> 16) & 0xff;
		for(int y3=y;y3<y2;y3++) {
			for (int x3=ox;x3<ox2;) {

#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(&p2[x3]) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/3;
		if (dwByte<0 || x>=lpSrc->m_nSizeX || y>=lpSrc->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBlt�̈ᔽ");
			break;
		}
	}
#endif
				if (p2[x3]!=r || p2[x3+1]!=g || p2[x3+2]!=b ||
					(dwColorKey==CLR_INVALID)) {
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(&p[x3]) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/3;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBlt�̈ᔽ");
			break;
		}
	}
#endif
					p[x3] = ((p2[x3]*ar)+(p[x3]*br))>>8; x3++;
					p[x3] = ((p2[x3]*ag)+(p[x3]*bg))>>8; x3++;
					p[x3] = ((p2[x3]*ab)+(p[x3]*bb))>>8; x3++;
				} else {
					x3+=3;
				}
			}
			p  = (BYTE*)(p +lPitch ); // �P���X�^���̑���
			p2 = (BYTE*)(p2+lPitch2);
			// BYTE�ɃL���X�g���Ă����Ȃ��ƌv�Z�ԈႤ:p
		}
	} break;

	case 32: {
		DWORD RMask, GMask, BMask,RGBMask;

		RMask = dddesc.ddpfPixelFormat.dwRBitMask;
		GMask = dddesc.ddpfPixelFormat.dwGBitMask;
		BMask = dddesc.ddpfPixelFormat.dwBBitMask;
		RGBMask = RMask | GMask | BMask;

		DWORD *p  = (DWORD*)dddesc.lpSurface;
		DWORD *p2 = (DWORD*)dddesc2.lpSurface;

		p  = (DWORD*)((BYTE*)p +(lPitch *y));	// �Z�J���_���̓]���J�ny���W
		p2 = (DWORD*)((BYTE*)p2+(lPitch2*sr.top)) + sr.left - x; // �������

		for(int y3=y;y3<y2;y3++) {
			for (int x3=x;x3<x2;x3++) {
				DWORD pixel, px,px2;

#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(&p2[x3]) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/4;
		if (dwByte<0 || x>=lpSrc->m_nSizeX || y>=lpSrc->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBlt�̈ᔽ");
			break;
		}
	}
#endif
				px2 = p2[x3];
				if ((px2&RGBMask)!=(dwColorKey&RGBMask)
					|| (dwColorKey==CLR_INVALID)){

#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(&p[x3]) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/4;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBlt�̈ᔽ");
			break;
		}
	}
#endif
					px	= p [x3];
					// �����ӂ�N�������[�H�V�����Ȃ��Ȃ�...
					// DWORDLONG�������o���̂͂�����Ƒ�l���Ȃ��悤�ȋC�����邯��:p
					pixel  = (DWORD)((((DWORDLONG)(px2&RMask)*ar)+((DWORDLONG)(px&RMask)*br))>>8)&RMask;
					pixel |= (DWORD)((((DWORDLONG)(px2&GMask)*ag)+((DWORDLONG)(px&GMask)*bg))>>8)&GMask;
					pixel |= (DWORD)((((DWORDLONG)(px2&BMask)*ab)+((DWORDLONG)(px&BMask)*bb))>>8)&BMask;
					p[x3]  = pixel;
				}
			}
			p  = (DWORD*)((BYTE*)p + lPitch); // �P���X�^���̑���
			p2 = (DWORD*)((BYTE*)p2+lPitch2);
		}

	} break; // end case
	} // end switch
	lpSrcSurface->Unlock(NULL);
	m_lpSurface->Unlock(NULL);

	return 0;
}

LRESULT		CPlane::BlendBltFast(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect,LPRECT lpClipDstRect){
	if (m_lpSurface==NULL) return -1;

	DRAW_CLIPPER;

	WARNING(sr.left<0 || sr.right>lpSrc->m_nSizeX || sr.top<0 || sr.bottom>lpSrc->m_nSizeY,
		"CPlane::BlendBlt�̓]�����]������`�O����s�Ȃ��Ă��܂�");

	//	�u�����h�䗦�̏�E�`�F�b�N
	WARNING(ar+br>256,"CPlane::BlendBlt��ar+br����E���z���Ă��܂�");
	WARNING(ag+bg>256,"CPlane::BlendBlt��ag+bg����E���z���Ă��܂�");
	WARNING(ab+bb>256,"CPlane::BlendBlt��ab+bb����E���z���Ă��܂�");

	// DstSurface��lock
	DDSURFACEDESC dddesc;
	ZERO(dddesc); // �ꉞ��
	dddesc.dwSize = sizeof(dddesc);
	if (m_lpSurface->Lock(NULL,&dddesc,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::BlendBlt��Surface��Lock�Ɏ��s");
		return 2;
	}

	// SrcSurface��lock
	DDSURFACEDESC dddesc2;
	ZERO(dddesc2);
	dddesc2.dwSize = sizeof(dddesc2);
	if (lpSrcSurface->Lock(NULL,&dddesc2,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::BlendBlt��Surface��Lock�Ɏ��s");
		return 3;
	}

	// 1���X�^���̑���
	LONG lPitch	 = dddesc.lPitch;
	LONG lPitch2 = dddesc2.lPitch;

	switch (dddesc.ddpfPixelFormat.dwRGBBitCount) {
	case 4:	//	16�F��Ή�
	case 8:	//	256�F��Ή�
		break;

	case 16: { // 65536�F���[�h
		WORD RMask, GMask, BMask,RGBMask;

		RMask = (WORD)dddesc.ddpfPixelFormat.dwRBitMask;
		GMask = (WORD)dddesc.ddpfPixelFormat.dwGBitMask;
		BMask = (WORD)dddesc.ddpfPixelFormat.dwBBitMask;
		RGBMask = RMask | GMask | BMask;

		WORD *p	 = (WORD*)dddesc.lpSurface;
		WORD *p2 = (WORD*)dddesc2.lpSurface;

		p  = (WORD*)((BYTE*)p +(lPitch *y));	// �Z�J���_���̓]���J�ny���W
		p2 = (WORD*)((BYTE*)p2+(lPitch2*sr.top)) + sr.left - x;	// �������

		for(int y3=y;y3<y2;y3++) {
			for (int x3=x;x3<x2;x3++) {
				WORD pixel, px,px2;

#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(&p2[x3]) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/2;
		if (dwByte<0 || x>=lpSrc->m_nSizeX || y>=lpSrc->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltFast�̈ᔽ");
			break;
		}
	}
#endif
				px2= p2[x3];

//				if ((px2&RGBMask)!=(dwColorKey /* &RGBMask*/ )
//					/* || (dwColorKey==CLR_INVALID)*/ ){	// �]�����J���[�L�[
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(&p[x3]) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/2;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltFast�̈ᔽ");
			break;
		}
	}
#endif
					px = p [x3];
					pixel  = ((WORD)((((px2&RMask)*ar)+((px&RMask)*br))>>8)&RMask);
					pixel |= ((WORD)((((px2&GMask)*ag)+((px&GMask)*bg))>>8)&GMask);
					pixel |= ((WORD)((((px2&BMask)*ab)+((px&BMask)*bb))>>8)&BMask);
					p[x3] = pixel;
//				}
			}
			p  = (WORD*)((BYTE*)p +lPitch ); // �P���X�^���̑���
			p2 = (WORD*)((BYTE*)p2+lPitch2);
			// BYTE�ɃL���X�g���Ă����Ȃ��ƌv�Z�ԈႤ:p
		}
	} break;

	case 24: {
		BYTE *p	 = (BYTE*)dddesc.lpSurface;
		BYTE *p2 = (BYTE*)dddesc2.lpSurface;

		p  = (BYTE*)p +(lPitch*y);	// �Z�J���_���̓]���J�ny���W
		p2 = (BYTE*)p2+(lPitch2*sr.top) + (sr.left - x)*3; // �������

		int ox=x*3,ox2=x2*3;

//		BYTE r,g,b;
//		r = dwColorKey & 0xff;
//		g = (dwColorKey >> 8) & 0xff;
//		b = (dwColorKey >> 16) & 0xff;
		for(int y3=y;y3<y2;y3++) {
			for (int x3=ox;x3<ox2;) {
#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(&p2[x3]) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/3;
		if (dwByte<0 || x>=lpSrc->m_nSizeX || y>=lpSrc->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltFast�̈ᔽ");
			break;
		}
	}
#endif

#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(&p[x3]) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/3;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltFast�̈ᔽ");
			break;
		}
	}
#endif
//				if (p2[x3]!=r || p2[x3+1]!=g || p2[x3+2]!=b
//					/* || (dwColorKey==CLR_INVALID) */ ) {
					p[x3] = ((p2[x3]*ar)+(p[x3]*br))>>8; x3++;
					p[x3] = ((p2[x3]*ag)+(p[x3]*bg))>>8; x3++;
					p[x3] = ((p2[x3]*ab)+(p[x3]*bb))>>8; x3++;
//				} else {
//					x3+=3;
//				}
			}
			p  = (BYTE*)(p +lPitch ); // �P���X�^���̑���
			p2 = (BYTE*)(p2+lPitch2);
			// BYTE�ɃL���X�g���Ă����Ȃ��ƌv�Z�ԈႤ:p
		}
	} break;

	case 32: {
		DWORD RMask, GMask, BMask,RGBMask;

		RMask = dddesc.ddpfPixelFormat.dwRBitMask;
		GMask = dddesc.ddpfPixelFormat.dwGBitMask;
		BMask = dddesc.ddpfPixelFormat.dwBBitMask;
		RGBMask = RMask | GMask | BMask;

		DWORD *p  = (DWORD*)dddesc.lpSurface;
		DWORD *p2 = (DWORD*)dddesc2.lpSurface;

		p  = (DWORD*)((BYTE*)p +(lPitch *y));	// �Z�J���_���̓]���J�ny���W
		p2 = (DWORD*)((BYTE*)p2+(lPitch2*sr.top)) + sr.left - x; // �������

		for(int y3=y;y3<y2;y3++) {
			for (int x3=x;x3<x2;x3++) {
				DWORD pixel, px,px2;
#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(&p2[x3]) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/4;
		if (dwByte<0 || x>=lpSrc->m_nSizeX || y>=lpSrc->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltFast�̈ᔽ");
			break;
		}
	}
#endif
				px2 = p2[x3];
//				if ((px2&RGBMask)!=(dwColorKey /* &RGBMask */ )
//					/* || (dwColorKey==CLR_INVALID) */ ){
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(&p[x3]) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/4;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltFast�̈ᔽ");
			break;
		}
	}
#endif
					px	= p [x3];
					// �����ӂ�N�������[�H�V�����Ȃ��Ȃ�...
					// DWORDLONG�������o���̂͂�����Ƒ�l���Ȃ��悤�ȋC�����邯��:p
					pixel  = (DWORD)((((DWORDLONG)(px2&RMask)*ar)+((DWORDLONG)(px&RMask)*br))>>8)&RMask;
					pixel |= (DWORD)((((DWORDLONG)(px2&GMask)*ag)+((DWORDLONG)(px&GMask)*bg))>>8)&GMask;
					pixel |= (DWORD)((((DWORDLONG)(px2&BMask)*ab)+((DWORDLONG)(px&BMask)*bb))>>8)&BMask;
					p[x3]  = pixel;
//				}
			}
			p  = (DWORD*)((BYTE*)p + lPitch); // �P���X�^���̑���
			p2 = (DWORD*)((BYTE*)p2+lPitch2);
		}

	} break; // end case
	} // end switch
	lpSrcSurface->Unlock(NULL);
	m_lpSurface->Unlock(NULL);

	return 0;
}


////////////////////////////////////////////////////////////////////
//	�g�k���݂̓]�����[�`��
//		programmed by Tia Deen

//////////////////////////////////////////////////////////////////
#define DRAW_CLIPPER_R \
	RECT clip;													\
	if (lpClipDstRect==NULL){									\
		::SetRect(&clip,0,0,m_nSizeX,m_nSizeY);					\
	} else {													\
		clip = *lpClipDstRect;									\
	}															\
	RECT sr;													\
	if (lpSrcRect==NULL){										\
		::SetRect(&sr,0,0,lpSrc->m_nSizeX,lpSrc->m_nSizeY);		\
	} else {													\
		sr	= *lpSrcRect;										\
	}															\
	RECT dr;													\
	if (lpDstSize==NULL){										\
		/* lpDstSize==NULL�Ȃ�΁Adst�v���[���S�悪�Ώ�	 */		\
		::SetRect(&dr,0,0,m_nSizeX,m_nSizeY);					\
	} else {													\
		::SetRect( &dr,x,y,x+lpDstSize->cx,y+lpDstSize->cy );	\
	}															\
	LPDIRECTDRAW lpDraw = CAppManager::GetMyDirectDraw()->GetDDraw();	\
	if (lpDraw==NULL) return 1;									\
	if (m_lpSurface==NULL) return 2;							\
	LPDIRECTDRAWSURFACE lpSrcSurface = lpSrc->GetSurface();		\
	if (lpSrcSurface==NULL) return 3;							\
	/*	�u���[���n���̏����l���v�Z���� */						\
	int		nInitialX, nInitialY;		/* -DX :�@�Â̏����l = -DX*/			\
	int		nStepsX,	 nStepsY;		/* SrcX�̈��̉��Z��(������)*/			\
	int		nStepX,	 nStepY;			/* SX :�@��+=SX	 */					\
	int		nCmpX,	 nCmpY;				/* DX :�@��>0�Ȃ�΃�-=DX���Ă�*/	\
	nInitialX = (dr.left - dr.right);							\
	nInitialY = (dr.top	- dr.bottom);							\
	nStepX	= (sr.right	 - sr.left) ;							\
	nStepY	= (sr.bottom - sr.top ) ;							\
	nCmpX = - (nInitialX );										\
	nCmpY = - (nInitialY );										\
	/* invalid rect */											\
	if (nCmpX == 0 || nCmpY == 0) return 4;						\
		/* �N���b�s���O���� */									\
		/* this clipping algorithm is thought by yaneurao(M.Isozaki) */	\
		int t;													\
		t = clip.left-dr.left;									\
		if (t>0)	{											\
			nInitialX+=t*nStepX;								\
			if (nInitialX > 0){									\
				int s = nInitialX / nCmpX +1;					\
				sr.left += s;									\
				nInitialX		-= s*nCmpX;						\
			}													\
			dr.left	  = clip.left;								\
		}														\
		t = clip.top -dr.top;									\
		if (t>0)	{											\
			nInitialY+=t*nStepY;								\
			if (nInitialY > 0){									\
				int s = nInitialY / nCmpY +1;					\
				sr.top += s;									\
				nInitialY		-= s*nCmpY;						\
			}													\
			dr.top	= clip.top;									\
		}														\
		t = dr.right-clip.right;								\
		if (t>0)	{											\
			dr.right  = clip.right;								\
/*			int nInitialX2 = nInitialX;							\
			nInitialX2+=(dr.right-dr.left)*nStepX;				\
			if (nInitialX2 > 0){								\
				int s = nInitialX2 / nCmpX + 1;					\
				sr.right = sr.left + s;							\
			}													\
		} else {												\
			sr.right--;											\
*/		}														\
																\
		t = dr.bottom-clip.bottom;								\
		if (t>0)	{ /*m_rcSrcRect.bottom -= t;*/	dr.bottom = clip.bottom;}	\
		/*	invalid rect ? */									\
		if (sr.left >= sr.right ||								\
			sr.top	 >= sr.bottom ||							\
			dr.left >= dr.right ||								\
			dr.top	 >= dr.bottom) return 4;					\
		/*	nStepX < nCmpX��ۏ؂���B */						\
		nStepsX = nStepX/nCmpX;									\
		nStepX -= nCmpX*nStepsX;								\
		nStepsY = nStepY/nCmpY;									\
		nStepY -= nCmpY*nStepsY;								\
		/* �ǂ��łP�������̂�������Ȃ��̂ŁA�����ň����̂��� By Tia */\
		nInitialX--;											\
		nInitialY--;											\

// �N���b�p�[�͂����܂�
////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////
//	BltR
//	�J���[�L�[�L��A�g�k�L��]��
//////////////////////////////////////////////
LRESULT		CPlane::BltR(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipDstRect)
{
	if (m_lpSurface==NULL) return -1;

	{int		nSsizeX, nSsizeY, nDsizeX, nDsizeY;
	if (lpSrcRect==NULL) {
		nSsizeX = lpSrc->m_nSizeX;
		nSsizeY = lpSrc->m_nSizeY;
	} else {
		nSsizeX = lpSrcRect->right - lpSrcRect->left;
		nSsizeY = lpSrcRect->bottom - lpSrcRect->top;
	}
	if (lpDstSize==NULL) {
		nDsizeX = m_nSizeX;
		nDsizeY = m_nSizeY;
	} else {
		nDsizeX = lpDstSize->cx;
		nDsizeY = lpDstSize->cy;
	}
	if ((nSsizeX == nDsizeX) && (nSsizeY == nDsizeY))
		return CPlane::Blt(lpSrc, x, y, lpSrcRect, lpClipDstRect);
	}
// �����ݒ�
	// �N���b�s���O����
	DRAW_CLIPPER_R;
	WARNING(sr.left<0 || sr.right>lpSrc->m_nSizeX || sr.top<0 || sr.bottom>lpSrc->m_nSizeY,
		"CPlane::BltR�̓]�����]������`�O����s�Ȃ��Ă��܂�");

	// �J���[�L�[�̎擾
	DWORD	dwColorKey;
	DDCOLORKEY	ddck;
	if (lpSrcSurface->GetColorKey(DDCKEY_SRCBLT, &ddck)==DD_OK) {;	// ���߃L�[�𓾂�
		dwColorKey =	ddck.dwColorSpaceLowValue;
	} else {
		dwColorKey = CLR_INVALID; // ����������Ȃ���
		//	����...
	}

	// DstSurface��lock
	DDSURFACEDESC dddesc;
	ZERO(dddesc); // �ꉞ��
	dddesc.dwSize = sizeof(dddesc);
	if (m_lpSurface->Lock(NULL,&dddesc,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::BltR��Surface��Lock�Ɏ��s");
		return 2;
	}

	// SrcSurface��lock
	DDSURFACEDESC dddesc2;
	ZERO(dddesc2);
	dddesc2.dwSize = sizeof(dddesc2);
	if (lpSrcSurface->Lock(NULL,&dddesc2,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::BltR��Surface��Lock�Ɏ��s");
		return 3;
	}

	// 1���X�^���̑���
	LONG lPitchDst = dddesc.lPitch;
	LONG lPitchSrc = dddesc2.lPitch;

#ifdef VRAM_MEMORY_CHECK
	CPlane* lpSrcOrg = lpSrc;	//	�ۑ����Ƃ��Ȃ���O�O�G
#endif	

	switch ( dddesc.ddpfPixelFormat.dwRGBBitCount )
	{
		case 4:
			break;

		case 8:
		{
			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;								// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth);							// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			BYTE*	lpSrc = (BYTE*)dddesc2.lpSurface +(sr.left)+sr.top*lPitchSrc;	// �N���b�s���O�����̃J�b�g
			BYTE*	lpDst = (BYTE*)dddesc.lpSurface + (dr.left)+dr.top*lPitchDst;	// �w�肳�ꂽx,y�̈ʒu����

			BYTE	nAddPixel = 1;
			BYTE	AddSrcPixel = nStepsX;
			BYTE	AddWidthSrc2 = sr.right - sr.left;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		i, j;
			int		nExCnt, nEyCnt;

			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				BYTE*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{

#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch);
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltR�̈ᔽ");
			break;
		}
	}
#endif
					BYTE	src = *lpSrc;
					// �J���[�L�[���ۂ��̔���
					if ( (src != dwColorKey) || (dwColorKey == CLR_INVALID) ) {
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch);
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltR�̈ᔽ");
			break;
		}
	}
#endif
						*lpDst = src;
					}
					lpSrc = lpSrc + AddSrcPixel;						// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = lpSrc + nAddPixel;						// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = lpDst + nAddPixel;
				}
				lpSrc = lpSrcBack + nAddSrcHeight;						// X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc += lPitchSrc;									// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst += nAddDstWidth;
			}
			break;
		}
		case 16:
		{
			WORD RGBMask = (WORD)dddesc.ddpfPixelFormat.dwRBitMask |
						   (WORD)dddesc.ddpfPixelFormat.dwGBitMask |
						   (WORD)dddesc.ddpfPixelFormat.dwBBitMask;

			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;						// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth<<1);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			WORD*	lpSrc = (WORD*)((BYTE*)dddesc2.lpSurface +(sr.left<<1)+sr.top*lPitchSrc );	// �N���b�s���O�����̃J�b�g
			WORD*	lpDst = (WORD*)((BYTE*)dddesc.lpSurface + (dr.left<<1)+dr.top*lPitchDst );	// �w�肳�ꂽx,y�̈ʒu����

			WORD	nAddPixel = 1 << 1;
			WORD	AddSrcPixel = nStepsX << 1;
			WORD	AddWidthSrc2 = (sr.right - sr.left)<<1;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		colKey = dwColorKey /* & RGBMask*/ ;
			int		i, j;
			int		nExCnt, nEyCnt;


			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				WORD*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{
#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/2;
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltR�̈ᔽ");
			break;
		}
	}
#endif
					WORD	src = *lpSrc;
					// �J���[�L�[���ۂ��̔���
					if ( ((src & RGBMask) != colKey) || (dwColorKey == CLR_INVALID) ){
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/2;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltR�̈ᔽ");
			break;
		}
	}
#endif
						*lpDst = src;
					}
					lpSrc = (WORD*)((BYTE*)lpSrc + AddSrcPixel);		// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = (WORD*)((BYTE*)lpSrc + nAddPixel);		// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = (WORD*)((BYTE*)lpDst + nAddPixel );
				}
				lpSrc = (WORD*)((BYTE*)lpSrcBack + nAddSrcHeight );		// X���[�v�Ői�񂾕��߂��Ay���̐����������Z���� (Thanks for Tear.)
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc = (WORD*)((BYTE*)lpSrc + lPitchSrc );			// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst = (WORD*)((BYTE*)lpDst + nAddDstWidth );
			}
			break;
		}
		case 24:
		{
			BYTE r,g,b;
			b =	 dwColorKey		   & 0xff;
			g = (dwColorKey >> 8 ) & 0xff;
			r = (dwColorKey >> 16) & 0xff;

			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;							// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth*3);						// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			BYTE*	lpSrc = (BYTE*)dddesc2.lpSurface +(sr.left*3)+sr.top*lPitchSrc;	// �N���b�s���O�����̃J�b�g
			BYTE*	lpDst = (BYTE*)dddesc.lpSurface + (dr.left*3)+dr.top*lPitchDst;	// �w�肳�ꂽx,y�̈ʒu����

			DWORD	nAddPixel = 1 * 3;
			DWORD	AddSrcPixel = nStepsX * 3;
			DWORD	AddWidthSrc2 = (sr.right - sr.left)*3;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		i, j;
			int		nExCnt, nEyCnt;


			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				BYTE*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{

#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/3;
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltR�̈ᔽ");
			break;
		}
	}
#endif
					BYTE	srcB, srcG, srcR;
					srcB = *lpSrc;
					srcG = *(lpSrc+1);
					srcR = *(lpSrc+2);
					// �J���[�L�[���ۂ��̔���
					if ( srcR != r || srcG != g || srcB != b || dwColorKey == CLR_INVALID )
					{
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/3;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltR�̈ᔽ");
			break;
		}
	}
#endif
						*lpDst	   = srcB;
						*(lpDst+1) = srcG;
						*(lpDst+2) = srcR;
					}
					lpSrc = lpSrc + AddSrcPixel;						// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = lpSrc + nAddPixel;						// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = lpDst + nAddPixel;
				}
				lpSrc = lpSrcBack + nAddSrcHeight;						// X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc += lPitchSrc;									// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst += nAddDstWidth;
			}
			break;
		}
		case 32:
		{
			DWORD RGBMask = dddesc.ddpfPixelFormat.dwRBitMask |
							dddesc.ddpfPixelFormat.dwGBitMask |
							dddesc.ddpfPixelFormat.dwBBitMask;

			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;						// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			DWORD*	lpSrc = (DWORD*)((BYTE*)dddesc2.lpSurface +(sr.left<<2)+sr.top*lPitchSrc );	// �N���b�s���O�����̃J�b�g
			DWORD*	lpDst = (DWORD*)((BYTE*)dddesc.lpSurface + (dr.left<<2)+dr.top*lPitchDst );	// �w�肳�ꂽx,y�̈ʒu����

			DWORD	nAddPixel = 1 << 2;
			DWORD	AddSrcPixel = nStepsX << 2;
			DWORD	AddWidthSrc2 = (sr.right - sr.left)<<2;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		colKey = dwColorKey /* & RGBMask*/ ;
			int		i, j;
			int		nExCnt, nEyCnt;


			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				DWORD*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{
#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/4;
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltR�̈ᔽ");
			break;
		}
	}
#endif
					DWORD	src = *lpSrc;
					// �J���[�L�[���ۂ��̔���
					if ( ((src & RGBMask) != colKey) || (dwColorKey == CLR_INVALID) ){
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/4;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltR�̈ᔽ");
			break;
		}
	}
#endif
						*lpDst = src;
					}
					lpSrc = (DWORD*)((BYTE*)lpSrc + AddSrcPixel);		// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = (DWORD*)((BYTE*)lpSrc + nAddPixel);		// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = (DWORD*)((BYTE*)lpDst + nAddPixel );
				}
				lpSrc = (DWORD*)((BYTE*)lpSrcBack + nAddSrcHeight );	// X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc = (DWORD*)((BYTE*)lpSrc + lPitchSrc );		// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst = (DWORD*)((BYTE*)lpDst + nAddDstWidth );
			}
			break;
		}
	}
	lpSrcSurface->Unlock( NULL );
	m_lpSurface->Unlock( NULL );


	return 0;
} // BltR


//////////////////////////////////////////////
//	BltFastR
//	�J���[�L�[�����A�g�k�L��]��
//////////////////////////////////////////////
LRESULT		CPlane::BltFastR(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipDstRect)
{
	if (m_lpSurface==NULL) return -1;

	{int		nSsizeX, nSsizeY, nDsizeX, nDsizeY;
	if (lpSrcRect==NULL) {
		nSsizeX = lpSrc->m_nSizeX;
		nSsizeY = lpSrc->m_nSizeY;
	} else {
		nSsizeX = lpSrcRect->right - lpSrcRect->left;
		nSsizeY = lpSrcRect->bottom - lpSrcRect->top;
	}
	if (lpDstSize==NULL) {
		nDsizeX = m_nSizeX;
		nDsizeY = m_nSizeY;
	} else {
		nDsizeX = lpDstSize->cx;
		nDsizeY = lpDstSize->cy;
	}
	if ((nSsizeX == nDsizeX) && (nSsizeY == nDsizeY))
		return CPlane::BltFast(lpSrc, x, y, lpSrcRect, lpClipDstRect);
	}
// �����ݒ�
	// �N���b�s���O����
	DRAW_CLIPPER_R;
	WARNING(sr.left<0 || sr.right>lpSrc->m_nSizeX || sr.top<0 || sr.bottom>lpSrc->m_nSizeY,
		"CPlane::BltFastR�̓]�����]������`�O����s�Ȃ��Ă��܂�");


	// DstSurface��lock
	DDSURFACEDESC dddesc;
	ZERO(dddesc); // �ꉞ��
	dddesc.dwSize = sizeof(dddesc);

	if (m_lpSurface->Lock(NULL,&dddesc,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::BltR��Surface��Lock�Ɏ��s");
		return 2;
	}

	// SrcSurface��lock
	DDSURFACEDESC dddesc2;
	ZERO(dddesc2);

	dddesc2.dwSize = sizeof(dddesc2);
	if (lpSrcSurface->Lock(NULL,&dddesc2,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::BltR��Surface��Lock�Ɏ��s");
		return 3;
	}

	// 1���X�^���̑���
	LONG lPitchDst = dddesc.lPitch;
	LONG lPitchSrc = dddesc2.lPitch;

#ifdef VRAM_MEMORY_CHECK
	CPlane* lpSrcOrg = lpSrc;	//	�ۑ����Ƃ��Ȃ���O�O�G
#endif	

	switch ( dddesc.ddpfPixelFormat.dwRGBBitCount )
	{
		case 4:
			break;

		case 8:
		{
			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;								// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth);							// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			BYTE*	lpSrc = (BYTE*)dddesc2.lpSurface +(sr.left)+sr.top*lPitchSrc;	// �N���b�s���O�����̃J�b�g
			BYTE*	lpDst = (BYTE*)dddesc.lpSurface + (dr.left)+dr.top*lPitchDst;	// �w�肳�ꂽx,y�̈ʒu����

			BYTE	nAddPixel = 1;
			BYTE	AddSrcPixel = nStepsX;
			BYTE	AddWidthSrc2 = sr.right - sr.left;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		i, j;
			int		nExCnt, nEyCnt;

			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				BYTE*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{
#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch);
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltFastR�̈ᔽ");
			break;
		}
	}
#endif
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch);
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltFastR�̈ᔽ");
			break;
		}
	}
#endif
					*lpDst = *lpSrc;
					lpSrc = lpSrc + AddSrcPixel;						// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = lpSrc + nAddPixel;						// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = lpDst + nAddPixel;
				}
				lpSrc = lpSrcBack + nAddSrcHeight;						// X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc += lPitchSrc;									// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst += nAddDstWidth;
			}
			break;
		}
		case 16:
		{
			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;						// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth<<1);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			WORD*	lpSrc = (WORD*)((BYTE*)dddesc2.lpSurface +(sr.left<<1)+sr.top*lPitchSrc );	// �N���b�s���O�����̃J�b�g
			WORD*	lpDst = (WORD*)((BYTE*)dddesc.lpSurface + (dr.left<<1)+dr.top*lPitchDst );	// �w�肳�ꂽx,y�̈ʒu����

			WORD	nAddPixel = 1 << 1;
			WORD	AddSrcPixel = nStepsX << 1;
			WORD	AddWidthSrc2 = (sr.right - sr.left)<<1;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		i, j;
			int		nExCnt, nEyCnt;


			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				WORD*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{
#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/2;
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltFastR�̈ᔽ");
			break;
		}
	}
#endif
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/2;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltFastR�̈ᔽ");
			break;
		}
	}
#endif
					*lpDst = *lpSrc;
					lpSrc = (WORD*)((BYTE*)lpSrc + AddSrcPixel);		// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = (WORD*)((BYTE*)lpSrc + nAddPixel);		// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = (WORD*)((BYTE*)lpDst + nAddPixel );
				}
				lpSrc = (WORD*)((BYTE*)lpSrcBack + nAddSrcHeight );		// X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc = (WORD*)((BYTE*)lpSrc + lPitchSrc );			// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst = (WORD*)((BYTE*)lpDst + nAddDstWidth );
			}
			break;
		}
		case 24:
		{
			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;							// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth*3);						// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			BYTE*	lpSrc = (BYTE*)dddesc2.lpSurface +(sr.left*3)+sr.top*lPitchSrc;	// �N���b�s���O�����̃J�b�g
			BYTE*	lpDst = (BYTE*)dddesc.lpSurface + (dr.left*3)+dr.top*lPitchDst;	// �w�肳�ꂽx,y�̈ʒu����

			DWORD	nAddPixel = 1 * 3;
			DWORD	AddSrcPixel = nStepsX * 3;
			DWORD	AddWidthSrc2 = (sr.right - sr.left)*3;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		i, j;
			int		nExCnt, nEyCnt;


			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				BYTE*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{
#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/3;
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltFastR�̈ᔽ");
			break;
		}
	}
#endif
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/3;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltFastR�̈ᔽ");
			break;
		}
	}
#endif
					*lpDst	  = *lpSrc;
					*(lpDst+1) = *(lpSrc+1);
					*(lpDst+2) = *(lpSrc+2);
					lpSrc = lpSrc + AddSrcPixel;						// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = lpSrc + nAddPixel;						// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = lpDst + nAddPixel;
				}
				lpSrc = lpSrcBack + nAddSrcHeight;						// X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc += lPitchSrc;									// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst += nAddDstWidth;
			}
			break;
		}
		case 32:
		{
			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;						// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			DWORD*	lpSrc = (DWORD*)((BYTE*)dddesc2.lpSurface +(sr.left<<2)+sr.top*lPitchSrc );	// �N���b�s���O�����̃J�b�g
			DWORD*	lpDst = (DWORD*)((BYTE*)dddesc.lpSurface + (dr.left<<2)+dr.top*lPitchDst );	// �w�肳�ꂽx,y�̈ʒu����

			DWORD	nAddPixel = 1 << 2;
			DWORD	AddSrcPixel = nStepsX << 2;
			DWORD	AddWidthSrc2 = (sr.right - sr.left)<<2;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		i, j;
			int		nExCnt, nEyCnt;


			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				DWORD*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{
#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/4;
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltFastR�̈ᔽ");
			break;
		}
	}
#endif
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/4;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BltFastR�̈ᔽ");
			break;
		}
	}
#endif
					*lpDst = *lpSrc;
					lpSrc = (DWORD*)((BYTE*)lpSrc + AddSrcPixel);		// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = (DWORD*)((BYTE*)lpSrc + nAddPixel);		// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = (DWORD*)((BYTE*)lpDst + nAddPixel );
				}
				lpSrc = (DWORD*)((BYTE*)lpSrcBack + nAddSrcHeight );	// X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc = (DWORD*)((BYTE*)lpSrc + lPitchSrc );		// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst = (DWORD*)((BYTE*)lpDst + nAddDstWidth );
			}
			break;
		}
	}
	lpSrcSurface->Unlock( NULL );
	m_lpSurface->Unlock( NULL );


	return 0;
} // BltFastR


//////////////////////////////////////////////
//	BlendBltR
//	�J���[�L�[�L���A�g�k�L��u�����h�]��
//////////////////////////////////////////////
LRESULT		CPlane::BlendBltR(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipDstRect)
{
	if (m_lpSurface==NULL) return -1;

	{int		nSsizeX, nSsizeY, nDsizeX, nDsizeY;
	if (lpSrcRect==NULL) {
		nSsizeX = lpSrc->m_nSizeX;
		nSsizeY = lpSrc->m_nSizeY;
	} else {
		nSsizeX = lpSrcRect->right - lpSrcRect->left;
		nSsizeY = lpSrcRect->bottom - lpSrcRect->top;
	}
	if (lpDstSize==NULL) {
		nDsizeX = m_nSizeX;
		nDsizeY = m_nSizeY;
	} else {
		nDsizeX = lpDstSize->cx;
		nDsizeY = lpDstSize->cy;
	}
	if ((nSsizeX == nDsizeX) && (nSsizeY == nDsizeY))
		return CPlane::BlendBlt(lpSrc, x, y, ar, ag, ab, br, bg, bb, lpSrcRect, lpClipDstRect);
	}
// �����ݒ�
	// �N���b�s���O����
	DRAW_CLIPPER_R;
	WARNING(sr.left<0 || sr.right>lpSrc->m_nSizeX || sr.top<0 || sr.bottom>lpSrc->m_nSizeY,
		"CPlane::BlendBltR�̓]�����]������`�O����s�Ȃ��Ă��܂�");


	// �J���[�L�[�̎擾
	DWORD	dwColorKey;
	DDCOLORKEY	ddck;
	if (lpSrcSurface->GetColorKey(DDCKEY_SRCBLT, &ddck)==DD_OK) {;	// ���߃L�[�𓾂�
		dwColorKey =	ddck.dwColorSpaceLowValue;
	} else {
		dwColorKey = CLR_INVALID; // ����������Ȃ���
	}

	// DstSurface��lock
	DDSURFACEDESC dddesc;
	ZERO(dddesc); // �ꉞ��
	dddesc.dwSize = sizeof(dddesc);
	if (m_lpSurface->Lock(NULL,&dddesc,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::BltR��Surface��Lock�Ɏ��s");
		return 2;
	}

	// SrcSurface��lock
	DDSURFACEDESC dddesc2;
	ZERO(dddesc2);
	dddesc2.dwSize = sizeof(dddesc2);
	if (lpSrcSurface->Lock(NULL,&dddesc2,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::BltR��Surface��Lock�Ɏ��s");
		return 3;
	}

	// 1���X�^���̑���
	LONG lPitchDst = dddesc.lPitch;
	LONG lPitchSrc = dddesc2.lPitch;

#ifdef VRAM_MEMORY_CHECK
	CPlane* lpSrcOrg = lpSrc;	//	�ۑ����Ƃ��Ȃ���O�O�G
#endif	

	switch ( dddesc.ddpfPixelFormat.dwRGBBitCount )
	{
		// �p���b�g�͔�T�|�[�g
		case 4:
		case 8:
			break;

		case 16:
		{
			WORD RMask, GMask, BMask,RGBMask;

			RMask = dddesc.ddpfPixelFormat.dwRBitMask;
			GMask = dddesc.ddpfPixelFormat.dwGBitMask;
			BMask = dddesc.ddpfPixelFormat.dwBBitMask;
			RGBMask = RMask | GMask | BMask;

			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;						// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth<<1);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			WORD*	lpSrc = (WORD*)((BYTE*)dddesc2.lpSurface +(sr.left<<1)+sr.top*lPitchSrc );	// �N���b�s���O�����̃J�b�g
			WORD*	lpDst = (WORD*)((BYTE*)dddesc.lpSurface + (dr.left<<1)+dr.top*lPitchDst );	// �w�肳�ꂽx,y�̈ʒu����

			WORD	nAddPixel = 1 << 1;
			WORD	AddSrcPixel = nStepsX << 1;
			WORD	AddWidthSrc2 = (sr.right - sr.left)<<1;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		colKey = dwColorKey /* & RGBMask */ ;
			int		i, j;
			int		nExCnt, nEyCnt;


			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				WORD*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{
#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/2;
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltR�̈ᔽ");
			break;
		}
	}
#endif
					WORD	src = *lpSrc;
					// �J���[�L�[���ۂ��̔���
					if ( ((src & RGBMask) != colKey) || (dwColorKey == CLR_INVALID) )
					{
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/2;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltR�̈ᔽ");
			break;
		}
	}
#endif
						// �����ӂ�N�������[�H�V�����Ȃ��Ȃ�...
						// DWORDLONG�������o���̂͂�����Ƒ�l���Ȃ��悤�ȋC�����邯��:p
						WORD	pixel, dst;
						dst = *lpDst;
						pixel  = (WORD)((((src&RMask)*ar)+((dst&RMask)*br))>>8)&RMask;
						pixel |= (WORD)((((src&GMask)*ag)+((dst&GMask)*bg))>>8)&GMask;
						pixel |= (WORD)((((src&BMask)*ab)+((dst&BMask)*bb))>>8)&BMask;
						*lpDst = pixel;
					}
					lpSrc = (WORD*)((BYTE*)lpSrc + AddSrcPixel);		// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = (WORD*)((BYTE*)lpSrc + nAddPixel);		// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = (WORD*)((BYTE*)lpDst + nAddPixel );
				}
				lpSrc = (WORD*)((BYTE*)lpSrcBack + nAddSrcHeight );		// X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc = (WORD*)((BYTE*)lpSrc + lPitchSrc );			// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst = (WORD*)((BYTE*)lpDst + nAddDstWidth );
			}
			break;
		}
		case 24:
		{
			DWORD r,g,b;
			b =	 dwColorKey		   & 0xff;
			g = (dwColorKey >> 8 ) & 0xff;
			r = (dwColorKey >> 16) & 0xff;

			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;						// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth*3);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			BYTE*	lpSrc = (BYTE*)dddesc2.lpSurface +(sr.left*3)+sr.top*lPitchSrc;	// �N���b�s���O�����̃J�b�g
			BYTE*	lpDst = (BYTE*)dddesc.lpSurface + (dr.left*3)+dr.top*lPitchDst;	// �w�肳�ꂽx,y�̈ʒu����

			DWORD	nAddPixel = 1 * 3;
			DWORD	AddSrcPixel = nStepsX*3;
			DWORD	AddWidthSrc2 = (sr.right - sr.left)*3;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		i, j;
			int		nExCnt, nEyCnt;


			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				BYTE*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{
#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/3;
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltR�̈ᔽ");
			break;
		}
	}
#endif
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/3;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltR�̈ᔽ");
			break;
		}
	}
#endif
					DWORD	srcR, srcG, srcB;
					srcB =	*lpSrc;
					srcG = *(lpSrc+1);
					srcR = *(lpSrc+2);
					// �J���[�L�[���ۂ��̔���
					if ( srcR != r || srcG != g || srcB != b || dwColorKey == CLR_INVALID )
					{
						DWORD	dstR, dstG, dstB;
						dstB =	*lpDst;
						dstG = *(lpDst+1);
						dstR = *(lpDst+2);
						 *lpDst		= ((srcB*ab)+(dstB*bb))>>8;
						*(lpDst+1)	= ((srcG*ag)+(dstG*bg))>>8;
						*(lpDst+2)	= ((srcR*ar)+(dstR*br))>>8;
					}
					lpSrc = lpSrc + AddSrcPixel;						// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = lpSrc + nAddPixel;						// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = lpDst + nAddPixel;
				}
				lpSrc = lpSrcBack + nAddSrcHeight;						// X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc += lPitchSrc;									// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst += nAddDstWidth;
			}
			break;
		}
		case 32:
		{
			DWORD RMask, GMask, BMask,RGBMask;

			RMask = dddesc.ddpfPixelFormat.dwRBitMask;
			GMask = dddesc.ddpfPixelFormat.dwGBitMask;
			BMask = dddesc.ddpfPixelFormat.dwBBitMask;
			RGBMask = RMask | GMask | BMask;

			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;						// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			DWORD*	lpSrc = (DWORD*)((BYTE*)dddesc2.lpSurface +(sr.left<<2)+sr.top*lPitchSrc );	// �N���b�s���O�����̃J�b�g
			DWORD*	lpDst = (DWORD*)((BYTE*)dddesc.lpSurface + (dr.left<<2)+dr.top*lPitchDst );	// �w�肳�ꂽx,y�̈ʒu����

			DWORD	nAddPixel = 1 << 2;
			DWORD	AddSrcPixel = nStepsX << 2;
			DWORD	AddWidthSrc2 = (sr.right - sr.left)<<2;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		colKey = dwColorKey /* & RGBMask*/ ;
			int		i, j;
			int		nExCnt, nEyCnt;


			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				DWORD*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{
#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/4;
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltR�̈ᔽ");
			break;
		}
	}
#endif
					DWORD	src = *lpSrc;
					// �J���[�L�[���ۂ��̔���
					if ( ((src & RGBMask) != colKey) || (dwColorKey == CLR_INVALID) )
					{
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/4;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltR�̈ᔽ");
			break;
		}
	}
#endif
						// �����ӂ�N�������[�H�V�����Ȃ��Ȃ�...
						// DWORDLONG�������o���̂͂�����Ƒ�l���Ȃ��悤�ȋC�����邯��:p
						DWORD	pixel, dst;
						dst = *lpDst;
						pixel = (DWORD)((((DWORDLONG)(src&RMask)*ar)+((DWORDLONG)(dst&RMask)*br))>>8)&RMask;
						pixel |= (DWORD)((((DWORDLONG)(src&GMask)*ag)+((DWORDLONG)(dst&GMask)*bg))>>8)&GMask;
						pixel |= (DWORD)((((DWORDLONG)(src&BMask)*ab)+((DWORDLONG)(dst&BMask)*bb))>>8)&BMask;
						*lpDst = pixel;
					}
					lpSrc = (DWORD*)((BYTE*)lpSrc + AddSrcPixel);		// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = (DWORD*)((BYTE*)lpSrc + nAddPixel);		// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = (DWORD*)((BYTE*)lpDst + nAddPixel );
				}
				lpSrc = (DWORD*)((BYTE*)lpSrcBack + nAddSrcHeight );	// X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc = (DWORD*)((BYTE*)lpSrc + lPitchSrc );		// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst = (DWORD*)((BYTE*)lpDst + nAddDstWidth );
			}
			break;
		}
	}
	lpSrcSurface->Unlock( NULL );
	m_lpSurface->Unlock( NULL );


	return 0;
} // BlendBltR



//////////////////////////////////////////////
//	BlendBltFastR
//	�J���[�L�[�L���A�g�k�L��u�����h�]��
//////////////////////////////////////////////
LRESULT		CPlane::BlendBltFastR(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipDstRect)
{
	if (m_lpSurface==NULL) return -1;

	{int		nSsizeX, nSsizeY, nDsizeX, nDsizeY;
	if (lpSrcRect==NULL) {
		nSsizeX = lpSrc->m_nSizeX;
		nSsizeY = lpSrc->m_nSizeY;
	} else {
		nSsizeX = lpSrcRect->right - lpSrcRect->left;
		nSsizeY = lpSrcRect->bottom - lpSrcRect->top;
	}
	if (lpDstSize==NULL) {
		nDsizeX = m_nSizeX;
		nDsizeY = m_nSizeY;
	} else {
		nDsizeX = lpDstSize->cx;
		nDsizeY = lpDstSize->cy;
	}
	if ((nSsizeX == nDsizeX) && (nSsizeY == nDsizeY))
		return CPlane::BlendBltFast(lpSrc, x, y, ar, ag, ab, br, bg, bb, lpSrcRect, lpClipDstRect);
	}
// �����ݒ�
	// �N���b�s���O����
	DRAW_CLIPPER_R;
	WARNING(sr.left<0 || sr.right>lpSrc->m_nSizeX || sr.top<0 || sr.bottom>lpSrc->m_nSizeY,
		"CPlane::BlendBltR�̓]�����]������`�O����s�Ȃ��Ă��܂�");

	// DstSurface��lock
	DDSURFACEDESC dddesc;
	ZERO(dddesc); // �ꉞ��
	dddesc.dwSize = sizeof(dddesc);
	if (m_lpSurface->Lock(NULL,&dddesc,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::BltR��Surface��Lock�Ɏ��s");
		return 2;
	}

	// SrcSurface��lock
	DDSURFACEDESC dddesc2;
	ZERO(dddesc2);
	dddesc2.dwSize = sizeof(dddesc2);
	if (lpSrcSurface->Lock(NULL,&dddesc2,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::BltR��Surface��Lock�Ɏ��s");
		return 3;
	}

	// 1���X�^���̑���
	LONG lPitchDst = dddesc.lPitch;
	LONG lPitchSrc = dddesc2.lPitch;
	
#ifdef VRAM_MEMORY_CHECK
	CPlane* lpSrcOrg = lpSrc;	//	�ۑ����Ƃ��Ȃ���O�O�G
#endif	

	switch ( dddesc.ddpfPixelFormat.dwRGBBitCount )
	{
		// �p���b�g�͔�T�|�[�g
		case 4:
		case 8:
			break;

		case 16:
		{
			WORD RMask, GMask, BMask,RGBMask;

			RMask = dddesc.ddpfPixelFormat.dwRBitMask;
			GMask = dddesc.ddpfPixelFormat.dwGBitMask;
			BMask = dddesc.ddpfPixelFormat.dwBBitMask;
			RGBMask = RMask | GMask | BMask;

			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;						// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth<<1);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			WORD*	lpSrc = (WORD*)((BYTE*)dddesc2.lpSurface +(sr.left<<1)+sr.top*lPitchSrc );	// �N���b�s���O�����̃J�b�g
			WORD*	lpDst = (WORD*)((BYTE*)dddesc.lpSurface + (dr.left<<1)+dr.top*lPitchDst );	// �w�肳�ꂽx,y�̈ʒu����

			WORD	nAddPixel = 1 << 1;
			WORD	AddSrcPixel = nStepsX << 1;
			WORD	AddWidthSrc2 = (sr.right - sr.left)<<1;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
//			int		colKey = dwColorKey & RGBMask;
			int		i, j;
			int		nExCnt, nEyCnt;


			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				WORD*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{

#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/2;
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltFastR�̈ᔽ");
			break;
		}
	}
#endif
					WORD	src = *lpSrc;
					// �J���[�L�[���ۂ��̔���
//					if ( ((src & RGBMask) != colKey) || (dwColorKey == CLR_INVALID) )
//					{
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/2;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltFastR�̈ᔽ");
			break;
		}
	}
#endif
						// �����ӂ�N�������[�H�V�����Ȃ��Ȃ�...
						// DWORDLONG�������o���̂͂�����Ƒ�l���Ȃ��悤�ȋC�����邯��:p
						WORD	pixel, dst;
						dst = *lpDst;
						pixel  = (WORD)((((src&RMask)*ar)+((dst&RMask)*br))>>8)&RMask;
						pixel |= (WORD)((((src&GMask)*ag)+((dst&GMask)*bg))>>8)&GMask;
						pixel |= (WORD)((((src&BMask)*ab)+((dst&BMask)*bb))>>8)&BMask;
						*lpDst = pixel;
//					}
					lpSrc = (WORD*)((BYTE*)lpSrc + AddSrcPixel);		// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = (WORD*)((BYTE*)lpSrc + nAddPixel);		// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = (WORD*)((BYTE*)lpDst + nAddPixel );
				}
				lpSrc = (WORD*)((BYTE*)lpSrcBack + nAddSrcHeight );		// X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc = (WORD*)((BYTE*)lpSrc + lPitchSrc );			// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst = (WORD*)((BYTE*)lpDst + nAddDstWidth );
			}
			break;
		}
		case 24:
		{
//			DWORD r,g,b;
//			b =	 dwColorKey		   & 0xff;
//			g = (dwColorKey >> 8 ) & 0xff;
//			r = (dwColorKey >> 16) & 0xff;

			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;						// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth*3);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			BYTE*	lpSrc = (BYTE*)dddesc2.lpSurface +(sr.left*3)+sr.top*lPitchSrc;	// �N���b�s���O�����̃J�b�g
			BYTE*	lpDst = (BYTE*)dddesc.lpSurface + (dr.left*3)+dr.top*lPitchDst;	// �w�肳�ꂽx,y�̈ʒu����

			DWORD	nAddPixel = 1 * 3;
			DWORD	AddSrcPixel = nStepsX*3;
			DWORD	AddWidthSrc = lPitchSrc * nStepsY;
			DWORD	AddWidthSrc2 = (sr.right - sr.left)*3;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		i, j;
			int		nExCnt, nEyCnt;


			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				BYTE*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{
#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/3;
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltFastR�̈ᔽ");
			break;
		}
	}
#endif
					DWORD	srcR, srcG, srcB;
					srcB =	*lpSrc;
					srcG = *(lpSrc+1);
					srcR = *(lpSrc+2);
					// �J���[�L�[���ۂ��̔���
//					if ( srcR != r || srcG != g || srcB != b || dwColorKey == CLR_INVALID )
//					{
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/3;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltFastR�̈ᔽ");
			break;
		}
	}
#endif
						DWORD	dstR, dstG, dstB;
						dstB =	*lpDst;
						dstG = *(lpDst+1);
						dstR = *(lpDst+2);
						 *lpDst		= ((srcB*ab)+(dstB*bb))>>8;
						*(lpDst+1)	= ((srcG*ag)+(dstG*bg))>>8;
						*(lpDst+2)	= ((srcR*ar)+(dstR*br))>>8;
//					}
					lpSrc = lpSrc + AddSrcPixel;						// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = lpSrc + nAddPixel;						// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = lpDst + nAddPixel;
				}
				lpSrc = lpSrcBack + nAddSrcHeight;						// X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc += lPitchSrc;									// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst += nAddDstWidth;
			}
			break;
		}
		case 32:
		{
			DWORD RMask, GMask, BMask,RGBMask;

			RMask = dddesc.ddpfPixelFormat.dwRBitMask;
			GMask = dddesc.ddpfPixelFormat.dwGBitMask;
			BMask = dddesc.ddpfPixelFormat.dwBBitMask;
			RGBMask = RMask | GMask | BMask;

			// �]����̉����Əc���̐ݒ�
			int		nWidth = dr.right - dr.left;
			int		nHeight = dr.bottom - dr.top;

			DWORD	nAddSrcHeight= lPitchSrc * nStepsY;						// y���̐������ŉ��Z�����l
			DWORD	nAddDstWidth = lPitchDst - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			DWORD*	lpSrc = (DWORD*)((BYTE*)dddesc2.lpSurface +(sr.left<<2)+sr.top*lPitchSrc );	// �N���b�s���O�����̃J�b�g
			DWORD*	lpDst = (DWORD*)((BYTE*)dddesc.lpSurface + (dr.left<<2)+dr.top*lPitchDst );	// �w�肳�ꂽx,y�̈ʒu����

			DWORD	nAddPixel = 1 << 2;
			DWORD	AddSrcPixel = nStepsX << 2;
			DWORD	AddWidthSrc2 = (sr.right - sr.left)<<2;
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
//			int		colKey = dwColorKey & RGBMask;
			int		i, j;
			int		nExCnt, nEyCnt;


			nEyCnt = EIY;
			for ( j = 0 ; j < nHeight ; j++ )
			{
				DWORD*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nWidth ; i++ )
				{
#ifdef VRAM_MEMORY_CHECK
	{	//	src check
		LONG dwByte = (BYTE*)(lpSrc) - (BYTE*)dddesc2.lpSurface;
		int	y = dwByte / dddesc2.lPitch;
		int x = (dwByte % dddesc2.lPitch)/4;
		if (dwByte<0 || x>=lpSrcOrg->m_nSizeX || y>=lpSrcOrg->m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltFastR�̈ᔽ");
			break;
		}
	}
#endif
					DWORD	src = *lpSrc;
					// �J���[�L�[���ۂ��̔���
//					if ( ((src & RGBMask) != colKey) || (dwColorKey == CLR_INVALID) )
//					{
#ifdef VRAM_MEMORY_CHECK
	{	//	dst check
		LONG dwByte = (BYTE*)(lpDst) - (BYTE*)dddesc.lpSurface;
		int	y = dwByte / dddesc.lPitch;
		int x = (dwByte % dddesc.lPitch)/4;
		if (dwByte<0 || x>=m_nSizeX || y>=m_nSizeY) {
			lpSrcSurface->Unlock(NULL);
			m_lpSurface->Unlock(NULL);
			WARNING(true,"CPlane::BlendBltFastR�̈ᔽ");
			break;
		}
	}
#endif
						// �����ӂ�N�������[�H�V�����Ȃ��Ȃ�...
						// DWORDLONG�������o���̂͂�����Ƒ�l���Ȃ��悤�ȋC�����邯��:p
						DWORD	pixel, dst;
						dst = *lpDst;
						pixel = (DWORD)((((DWORDLONG)(src&RMask)*ar)+((DWORDLONG)(dst&RMask)*br))>>8)&RMask;
						pixel |= (DWORD)((((DWORDLONG)(src&GMask)*ag)+((DWORDLONG)(dst&GMask)*bg))>>8)&GMask;
						pixel |= (DWORD)((((DWORDLONG)(src&BMask)*ab)+((DWORDLONG)(dst&BMask)*bb))>>8)&BMask;
						*lpDst = pixel;
//					}
					lpSrc = (DWORD*)((BYTE*)lpSrc + AddSrcPixel);		// �������̉��Z
					nExCnt += EX;										// X�̑���
					if ( nExCnt >= 0 )
					{
						lpSrc = (DWORD*)((BYTE*)lpSrc + nAddPixel);		// ���̃s�N�Z���ɂ���
						nExCnt -= EX2;									// X�̕␳�l
					}
					lpDst = (DWORD*)((BYTE*)lpDst + nAddPixel );
				}
				lpSrc = (DWORD*)((BYTE*)lpSrcBack + nAddSrcHeight );	// X���[�v�Ői�񂾕��߂��Ay���̐����������Z����1
				nEyCnt += EY;											// Y�̑���
				if ( nEyCnt >= 0 )
				{
					lpSrc = (DWORD*)((BYTE*)lpSrc + lPitchSrc );		// �N���b�s���O�Ŕ�΂����𑫂��āA���̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}
				lpDst = (DWORD*)((BYTE*)lpDst + nAddDstWidth );
			}
			break;
		}
	}
	lpSrcSurface->Unlock( NULL );
	m_lpSurface->Unlock( NULL );


	return 0;
} // BlendBltFastR

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//	Mosaic�i���̃v���[���ɑ΂���G�t�F�N�g�j
LRESULT CPlane::MosaicEffect(int d, LPRECT lpRect){
	//�@�v���[���ɑ΂��ă��U�C�N��������@�\
	//	d :	�ʎq�����x��
	LPDIRECTDRAWSURFACE lpSurface = GetSurface();

	if (m_lpSurface	 ==NULL) return -1;
	//	�蔲���ŃN���b�v���Ă��Ȃ��̂Łc

	if (d==0) return -2;	//	����ŗ�����̂͂܂����낤...

	RECT r;
	if(lpRect == NULL){
		::SetRect(&r,0,0,m_nSizeX,m_nSizeY);
	}else{
		r = *lpRect;
	}

	// �N���b�s���O����
	RECT rcClip;
	::SetRect(&rcClip,0,0,m_nSizeX,m_nSizeY);
	LPRECT lpClip = &rcClip;
	if (lpClip->left > r.left)	{ r.left   = lpClip->left;	 }
	if (lpClip->right< r.right) { r.right  = lpClip->right;	 }
	if (lpClip->top	 > r.top)	{ r.top	   = lpClip->top;	 }
	if (lpClip->bottom<r.bottom){ r.bottom = lpClip->bottom; }
	if (r.left >= r.right || r.top	>= r.bottom) return 1;

	//	lock the surface...
	DDSURFACEDESC dddesc;
	ZERO(dddesc);
	dddesc.dwSize = sizeof(dddesc);
	if (lpSurface->Lock(NULL,&dddesc,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::MosaicBlt��Surface��Lock�Ɏ��s");
		return 1;
	}

	// 1���X�^���̑���
	LONG lPitch	 = dddesc.lPitch;

	switch (dddesc.ddpfPixelFormat.dwRGBBitCount) {
	case 8:	{	// 256�F���[�h
		for(int y=r.top;y<r.bottom;y+=d){
			int d2;		//	���[�̒[��
			if (y+d>r.bottom) d2=r.bottom-y; else d2=d;
			for(int x=r.left;x<r.right;x+=d){
				int d1;	//	�E�[�̒[��
				if (x+d>r.right) d1=r.right-x; else d1=d;

				BYTE *p,*p2;
				p = (BYTE*)dddesc.lpSurface + y*lPitch + x;
				BYTE c;	// ��\�_�̐F
				c = *p;
				for(int py=0;py<d2;py++){
					p2 = p;
					for(int px=0;px<d1;px++){
						*(p++) = c;
					}
					p = p2 + lPitch;	//	next line
				}
			}
		}
		break;
			}
	case 16: {	// 65536�F���[�h
		for(int y=r.top;y<r.bottom;y+=d){
			int d2;		//	���[�̒[��
			if (y+d>r.bottom) d2=r.bottom-y; else d2=d;
			for(int x=r.left;x<r.right;x+=d){
				int d1;	//	�E�[�̒[��
				if (x+d>r.right) d1=r.right-x; else d1=d;

				WORD *p,*p2;
				p = (WORD*)((BYTE*)dddesc.lpSurface + y*lPitch) + x;
				WORD c;	// ��\�_�̐F
				c = *p;
				for(int py=0;py<d2;py++){
					p2 = p;
					for(int px=0;px<d1;px++){
						*(p++) = c;
					}
					p = (WORD*)((BYTE*)p2 + lPitch);	//	next line
				}
			}
		}
		break;
			 }
	case 24: {	//	full color
		for(int y=r.top;y<r.bottom;y+=d){
			int d2;		//	���[�̒[��
			if (y+d>r.bottom) d2=r.bottom-y; else d2=d;
			for(int x=r.left;x<r.right;x+=d){
				int d1;	//	�E�[�̒[��
				if (x+d>r.right) d1=r.right-x; else d1=d;

				BYTE *p,*p2;
				p = (BYTE*)dddesc.lpSurface + y*lPitch + x*3;
				BYTE c1,c2,c3;	// ��\�_�̐F
				c1 = *p; c2 = *(p+1); c3 = *(p+2);
				for(int py=0;py<d2;py++){
					p2 = p;
					for(int px=0;px<d1;px++){	//	�R�񂩂��Ƃ��`
						*(p++) = c1;
						*(p++) = c2;
						*(p++) = c3;
					}
					p = p2 + lPitch;	//	next line
				}
			}
		}
		break;
			 }
	case 32: {	//	true color
		for(int y=r.top;y<r.bottom;y+=d){
			int d2;		//	���[�̒[��
			if (y+d>r.bottom) d2=r.bottom-y; else d2=d;
			for(int x=r.left;x<r.right;x+=d){
				int d1;	//	�E�[�̒[��
				if (x+d>r.right) d1=r.right-x; else d1=d;

				DWORD *p,*p2;
				p = (DWORD*)((BYTE*)dddesc.lpSurface + y*lPitch) + x;
				DWORD c;	// ��\�_�̐F
				c = *p;
				for(int py=0;py<d2;py++){
					p2 = p;
					for(int px=0;px<d1;px++){
						*(p++) = c;
					}
					p = (DWORD*)((BYTE*)p2 + lPitch);	//	next line
				}
			}
		}
		break;
			}
	} // end switch
	lpSurface->Unlock(NULL);
	return 0;
}

//	Flush �i���̃v���[���ɑ΂���G�t�F�N�g�j
LRESULT CPlane::FlushEffect(LPRECT lpRect){
	//	�v���[���ɑ΂��ăl�K�|�W���]������@�\
	LPDIRECTDRAWSURFACE lpSurface = GetSurface();

	if (lpSurface	 ==NULL) return -1;
	//	�蔲���ŃN���b�v���Ă��Ȃ��̂Łc

	RECT r;
	if(lpRect == NULL){
		::SetRect(&r,0,0,m_nSizeX,m_nSizeY);
	}else{
		r = *lpRect;
	}

	// �N���b�s���O����
	RECT rcClip;
	::SetRect(&rcClip,0,0,m_nSizeX,m_nSizeY);
	LPRECT lpClip = &rcClip;
	if (lpClip->left > r.left)	{ r.left   = lpClip->left;	 }
	if (lpClip->right< r.right) { r.right  = lpClip->right;	 }
	if (lpClip->top	 > r.top)	{ r.top	   = lpClip->top;	 }
	if (lpClip->bottom<r.bottom){ r.bottom = lpClip->bottom; }
	if (r.left >= r.right || r.top	>= r.bottom) return 1;

	//	lock the surface...
	DDSURFACEDESC dddesc;
	ZERO(dddesc);
	dddesc.dwSize = sizeof(dddesc);
	if (lpSurface->Lock(NULL,&dddesc,
		DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK){
		Err.Out("CPlane::FlushBlt��Surface��Lock�Ɏ��s");
		return 1;
	}

	// 1���X�^���̑���
	LONG lPitch	 = dddesc.lPitch;

	switch (dddesc.ddpfPixelFormat.dwRGBBitCount) {
	case 8:	{	// 256�F���[�h
		for(int y=r.top;y<r.bottom;y++){
			BYTE *p;
			p = (BYTE*)dddesc.lpSurface + y*lPitch + r.left;
			for(int x=r.left;x<r.right;x++){
				*(p++) ^= 0xff;	//	xor���邾��:p
			}
		}
		break;
			}
	case 16: {	// 65536�F���[�h
		for(int y=r.top;y<r.bottom;y++){
			WORD *p;
			p = (WORD*)((BYTE*)dddesc.lpSurface + y*lPitch) + r.left;
			for(int x=r.left;x<r.right;x++){
				*(p++) ^= 0xffff;	//	xor���邾��:p
			}
		}
		break;
			 }
	case 24: {	//	full color
		for(int y=r.top;y<r.bottom;y++){
			BYTE *p;
			p = (BYTE*)dddesc.lpSurface + y*lPitch + r.left*3;
			for(int x=r.left;x<r.right;x++){	//	3�񂩂��Ƃ��`:p
				*(p++) ^= 0xff;	//	xor���邾��:p
				*(p++) ^= 0xff;
				*(p++) ^= 0xff;
			}
		}
		break;
			 }
	case 32: {	//	true color
		for(int y=r.top;y<r.bottom;y++){
			DWORD *p;
			p = (DWORD*)((BYTE*)dddesc.lpSurface + y*lPitch) + r.left;
			for(int x=r.left;x<r.right;x++){
				*(p++) ^= 0xffffff;	//	����24�r�b�g�ɑ΂���xor���邾��:p
			}
		}
		break;
			}
	} // end switch
	lpSurface->Unlock(NULL);
	return 0;
}

#endif // USE_DirectDraw