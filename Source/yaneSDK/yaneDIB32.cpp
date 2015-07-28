#include "stdafx.h"

#ifdef USE_DIB32

#include "yaneDIB32.h"
#include "yaneDIBitmap.h"
#include "yaneGraphicLoader.h"
#include "yaneCPUID.h"
#include "yanePlane.h"
#include "yaneFile.h"
#include "yaneSinTable.h"
#include "yaneLZSS.h"
#include "yaneAppManager.h"

///////////////////////////////////////////////////////////////////////////////
//	CDIBBase�̑㗝��

CDIB32::CDIB32(){
	//	Pentium�p�̃R�[�h���f�o�b�O����Ƃ��́A�����L���ɂ���
	//	m_lpDIB.Add(new CDIB32P5); return ;

	switch (CCPUID::GetID()){
	case 1:
	case 2:
		m_lpDIB.Add(new CDIB32P5);	 break;
	case 3:
		m_lpDIB.Add(new CDIB32PMMX); break;
	case 4:
		m_lpDIB.Add(new CDIB32P6);	 break;
	default:
		WARNING(true,"CPUID�����Ɏ��s"); break;
	}
}

CDIB32::~CDIB32(){
	m_lpDIB.Delete();
}

///////////////////////////////////////////////////////////////////////////////

LRESULT CDIB32Base::CreateSurface(int nSizeX,int nSizeY){
	LRESULT hr;
	hr = InnerCreateSurface(nSizeX,nSizeY);
	if (hr) return hr;
	return Clear();
}

LRESULT CDIB32Base::InnerCreateSurface(int nSizeX,int nSizeY){

	Release();	//	�m�ۂ��Ă���DIB�����

	if (nSizeX<=0 || nSizeY<=0) {
		WARNING(true,"CDIB32Base::CreateSurface�ŃT�C�Y����������");
		return 1;		//	invalid rect...����Ȃ�œn���Ȃ�[�O�O
	}

	if (m_bUseDIBSection) {
		//	DIBSection�ɗ���ꍇ
		BITMAPINFO bmi;
		bmi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth		=  nSizeX;
		bmi.bmiHeader.biHeight		= -nSizeY;
		bmi.bmiHeader.biPlanes		= 1;
		bmi.bmiHeader.biBitCount	= 32;
		bmi.bmiHeader.biCompression	= BI_RGB /* BI_BITFIELDS */ ;

		HDC hdc = ::GetDC(NULL); // hWnd�̂ق��������񂩁H
		m_hBitmap = ::CreateDIBSection(hdc /* NULL */, &bmi , DIB_RGB_COLORS, (void**)&m_lpdwSrc, NULL, 0 );
		::ReleaseDC(NULL,hdc);

		//	���̑�P�p�����[�^�́ANULL�Ƃ����̂͂܂����Ȃ����H
		//	GetDC(NULL)�ɂ��邩�A����Ƃ����O��32bpp�ŏ��������ꂽ
		//	DC���g���ׂ��̂悤�ȁA�A
		if (m_hBitmap==NULL) return 1;
		m_hDC = ::CreateCompatibleDC(NULL);
		::SelectObject(m_hDC,m_hBitmap);

/*
	����NULL��CompatibleDC������Ă�̂�������̂�ȁA�A�A
	NULL�͂a�f��ʂ��Ӗ�����̂����A
	���݂̉�ʉ𑜓x�̉e���������Ɏ󂯂�킯�ŁA
	����DC�ƃR���p�`�Ƃ������Ƃ́A�A���Ȃ킿����DC�́ADevice�ɑ΂���
	Independenct�ł����ł��������Ă��Ƃ₪�ȁD�D

	����[���Ƃ́A��ʉ𑜓x�̕ύX�C�x���g�̔����ɍۂ��āA���̂ւ�n���h����
	�Ƃ�Ȃ����������K�v�ɂȂ��Ă���킯��B���ꂪ���Ȃ�A����ȃR���p�`�u����DC�Ȃ񂩍��񂱂Ƃ�B
	����[�Ă��A�ǂ����悤��������₯�ǂȁB
	����[�Ȃ�����A���Ȃ����邩�A�A�݂����ȁA�A
*/

	} else {
		//	������DIB�����ꍇ
		int nAlign = 8;
		//	����σ��������������Ȃ�����8�Œ�ɂ��Ƃ�����:p
		/*
		if (CCPUID::GetID()<=3) {
			nAlign = 8;		//	Plain Pentium,MMX Pentium��QWORD�ŃA���C������
		} else {
			nAlign = 32;	//	Pentium Pro�ȍ~��32�o�C�g�ŃA���C������
		}
		*/
		m_lpdwSrcOrg	= new BYTE[4 * nSizeX * nSizeY + (nAlign-1)];
		m_lpdwSrc		= (DWORD*)(((DWORD)m_lpdwSrcOrg + (nAlign-1)) & ~(nAlign-1));
	}

	//	Rect�T�C�Y�̐ݒ�
	::SetRect(&m_rcRect,0,0,nSizeX,nSizeY);
	m_lPitch = m_rcRect.right << 2;

	//	�e���C���̐擪�A�h���X�e�[�u����p��
	m_lpdwLineStart.resize(nSizeY);
	{
		DWORD* lpdw = m_lpdwSrc;
		for(int i=0;i<nSizeY;i++){
			m_lpdwLineStart[i] = lpdw;
			lpdw = (DWORD*)((BYTE*)lpdw + m_lPitch);
		}
	}

	return 0;
}

LRESULT CDIB32Base::Resize(int nSizeX,int nSizeY){
	//	�T�C�Y���Ⴄ�Ȃ��Recreate
	if (m_rcRect.right!=nSizeX || m_rcRect.bottom!=nSizeY) {
		return CreateSurface(nSizeX,nSizeY);
	}
	return 0;
}

LPRECT	CDIB32Base::GetRect(){
	//	���̃v���[���̃T�[�t�F�[�X��`�擾
	return &m_rcRect;
}

RECT	CDIB32Base::GetClipRect(LPRECT lpRect){
	RECT r;
	if(lpRect == NULL){
		r = m_rcRect;
	} else {
		r = *lpRect;
	}

	// �N���b�s���O����
	LPRECT lpClip = &m_rcRect;

	if (lpClip->left > r.left)	{ r.left   = lpClip->left;	 }
	if (lpClip->right< r.right) { r.right  = lpClip->right;	 }
	if (lpClip->top	 > r.top)	{ r.top	   = lpClip->top;	 }
	if (lpClip->bottom<r.bottom){ r.bottom = lpClip->bottom; }

	//	invalid rect,but..
	//	if (r.left >= r.right || r.top	>= r.bottom) return 1;

	return r;
}

///////////////////////////////////////////////////////////////////////////////

void	CDIB32Base::UseDIBSection(bool bUseDIB){
	m_bUseDIBSection = bUseDIB;
}

//	CreateDIBSection���g���Ƃ��Ɍ���A�ȉ��̊֐����g�p�\
HDC		CDIB32Base::GetDC(void){
	return m_hDC;
}
HBITMAP	CDIB32Base::GetHBITMAP(void){
	return m_hBitmap;
}

#ifdef USE_DirectDraw
//	HDC�擾(����͕s�v�BLoad��CreateSurface�������Ƃ͉������܂Ŏ擾�\)
LRESULT	CDIB32Base::BltToPlane(CPlane*lpDstPlane,int x,int y,LPRECT lpSrcRect,LPRECT lpClipRect){
	WARNING(!m_bUseDIBSection,"CDIB32Base::BltToPlane�̌Ăяo�ɂ����āAUseDIBSection(true)����Ă��Ȃ�����");
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BltToPlane��m_lpdwSrc == NULL");

	//	generic clipping algorithm by yaneurao.

	//	�\�[�X��`��NULL�Ȃ�΁A�S��
	if (lpSrcRect==NULL) {
		m_rcSrcRect = m_rcRect;
	} else {
		m_rcSrcRect = *lpSrcRect;
	}

	//	�N���b�v�̈�
	LPRECT lpClip;
	RECT	rcClip;
	if (lpClipRect == NULL){
		int sx,sy;
		lpDstPlane->GetSize(sx,sy);
		::SetRect(&rcClip,0,0,sx,sy);
		lpClip = &rcClip;
	} else {
		lpClip = lpClipRect;

		/*	//	�߂�ǂ����[�܂��������`�O�O
		WARNING ( lpClip->left	< p->m_rcRect.left	|| lpClip->top	  < p->m_rcRect.top
			||	  lpClip->right > p->m_rcRect.right || lpClip->bottom > p->m_rcRect.bottom,
			"CDib32Base::Clipping�ŃN���b�v��`���]����T�[�t�F�[�X����͂ݏo���Ă���");
		*/
	}

	//--- �ǉ� '02/03/04  by ENRA ---
	{	// �N���b�vRect�́A�]����Rect�ɓ����Ȃ��Ƃ����Ȃ�
		// ������ClipRect���I�͂���ȏ��ɂ������ꍇ�̏����͂ǂ����悤�c
		//LPRECT lpDstRect2 = lpDstPlane->GetRect();	// CPlane�ɂ�GetRect�������c
		int sx,sy;  lpDstPlane->GetSize(sx,sy);
		int t;
		//--- �C�� '02/04/08  by ENRA ---
		// �����0�ɂ����炠���񂩂���^^;;
		// ���[��A0�ȉ��ɂȂ邱�Ƃ��Ă��蓾����납
		if (lpClip->left<0)	{ lpClip->left = 0; }
		if (lpClip->top <0)	{ lpClip->top  = 0; }
		//-------------------------------
		t = lpClip->right  - sx;
		if (t>0)	{ lpClip->right  = sx; }
		t = lpClip->bottom - sy;
		if (t>0)	{ lpClip->bottom = sy; }
	}
	//-------------------------------

	// �N���b�s���O����
	// this clipping algorithm is thought by yaneurao(M.Isozaki)
	int sx = m_rcSrcRect.right - m_rcSrcRect.left;
	int sy = m_rcSrcRect.bottom - m_rcSrcRect.top;
	::SetRect(&m_rcDstRect,x,y,x+sx,y+sy);

	int t;
	t = lpClip->left-x;
	if (t>0)	{ m_rcSrcRect.left += t;	m_rcDstRect.left = lpClip->left; }
	t = lpClip->top -y;
	if (t>0)	{ m_rcSrcRect.top  += t;	m_rcDstRect.top	 = lpClip->top;	 }
	t = x+sx-lpClip->right;
	if (t>0)	{ m_rcSrcRect.right -= t;	m_rcDstRect.right = lpClip->right; }
	t = y+sy-lpClip->bottom;
	if (t>0)	{ m_rcSrcRect.bottom -= t;	m_rcDstRect.bottom = lpClip->bottom; }

	//	invalid rect ?
	if (m_rcSrcRect.left >= m_rcSrcRect.right ||
		m_rcSrcRect.top	 >= m_rcSrcRect.bottom) return 1;

	HDC hDstDC= lpDstPlane->GetDC();
	if (hDstDC == NULL) return 2;	//	���ꎸ�s���邩�`

	LRESULT lr = ::BitBlt(hDstDC,m_rcDstRect.left,m_rcDstRect.top
		,m_rcSrcRect.right - m_rcSrcRect.left,m_rcSrcRect.bottom - m_rcSrcRect.top
		,GetDC(),m_rcSrcRect.left,m_rcSrcRect.top,SRCCOPY)?0:1;

	lpDstPlane->ReleaseDC();

	return lr;
}

LRESULT	CDIB32Base::BltFromPlane(CPlane*lpSrcPlane,int x,int y,LPRECT lpSrcRect,LPRECT lpClipRect){
	WARNING(!m_bUseDIBSection,"BltFromPlane::BltFromPlane�̌Ăяo�ɂ����āAUseDIBSection(true)����Ă��Ȃ�����");
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BltFromPlane��m_lpdwSrc == NULL");

	//	generic clipping algorithm by yaneurao.

	//	�\�[�X��`��NULL�Ȃ�΁A�S��
	if (lpSrcRect==NULL) {
		int sx,sy;
		lpSrcPlane->GetSize(sx,sy);
		::SetRect(&m_rcSrcRect,0,0,sx,sy);
	} else {
		m_rcSrcRect = *lpSrcRect;
	}

	//	�N���b�v�̈�
	LPRECT lpClip;
	if (lpClipRect == NULL){
		lpClip = &m_rcRect;
	} else {
		lpClip = lpClipRect;

		/*	//	�܂��������`�݂����ȁO�O
		WARNING ( lpClip->left	< p->m_rcRect.left	|| lpClip->top	  < p->m_rcRect.top
			||	  lpClip->right > p->m_rcRect.right || lpClip->bottom > p->m_rcRect.bottom,
			"CDib32Base::Clipping�ŃN���b�v��`���]����T�[�t�F�[�X����͂ݏo���Ă���");
		*/
	}

	//--- �ǉ� '02/03/04  by ENRA ---
	{	// �N���b�vRect�́A�]����Rect�ɓ����Ȃ��Ƃ����Ȃ�
		// ������ClipRect���I�͂���ȏ��ɂ������ꍇ�̏����͂ǂ����悤�c
		LPRECT lpDstRect2 = &m_rcRect;		// �]����͎���
		int t;
		//--- �C�� '02/04/08  by ENRA ---
		// �����0�ɂ����炠���񂩂���^^;;
		t = lpClip->left  - lpDstRect2->left;
		if (t<0)	{ lpClip->left  = lpDstRect2->left; }
		t = lpClip->top - lpDstRect2->top;
		if (t<0)	{ lpClip->top = lpDstRect2->top; }
		//-------------------------------
		t = lpClip->right  - lpDstRect2->right;
		if (t>0)	{ lpClip->right  = lpDstRect2->right; }
		t = lpClip->bottom - lpDstRect2->bottom;
		if (t>0)	{ lpClip->bottom = lpDstRect2->bottom; }
	}
	//-------------------------------


	// �N���b�s���O����
	// this clipping algorithm is thought by yaneurao(M.Isozaki)
	int sx = m_rcSrcRect.right - m_rcSrcRect.left;
	int sy = m_rcSrcRect.bottom - m_rcSrcRect.top;
	::SetRect(&m_rcDstRect,x,y,x+sx,y+sy);

	int t;
	t = lpClip->left-x;
	if (t>0)	{ m_rcSrcRect.left += t;	m_rcDstRect.left = lpClip->left; }
	t = lpClip->top -y;
	if (t>0)	{ m_rcSrcRect.top  += t;	m_rcDstRect.top	 = lpClip->top;	 }
	t = x+sx-lpClip->right;
	if (t>0)	{ m_rcSrcRect.right -= t;	m_rcDstRect.right = lpClip->right; }
	t = y+sy-lpClip->bottom;
	if (t>0)	{ m_rcSrcRect.bottom -= t;	m_rcDstRect.bottom = lpClip->bottom; }
	//	invalid rect ?
	if (m_rcSrcRect.left >= m_rcSrcRect.right ||
		m_rcSrcRect.top	 >= m_rcSrcRect.bottom) return 1;

	HDC hSrcDC= lpSrcPlane->GetDC();
	if (hSrcDC == NULL) return 2;	//	���ꎸ�s���邩�`

	LRESULT lr = ::BitBlt(GetDC(),m_rcDstRect.left,m_rcDstRect.top
		,m_rcSrcRect.right - m_rcSrcRect.left,m_rcSrcRect.bottom - m_rcSrcRect.top
		,hSrcDC,m_rcSrcRect.left,m_rcSrcRect.top,SRCCOPY)?0:1;

	lpSrcPlane->ReleaseDC();

	return lr;
}
#endif
///////////////////////////////////////////////////////////////////////////////

LRESULT CDIB32Base::Load(string BitmapFileName,bool bLoadPalette){
	Release();	//	�m�ۂ��Ă���DIB�����

#ifdef USE_YGA
	//	yga�t�H�[�}�b�g(yaneurao graphic format with alpha)
	string suffix;
	suffix = CFile::GetSuffixOf(BitmapFileName);
	CFile::ToLower(suffix);
	if (suffix=="yga"){
		LRESULT lr = InnerLoadYGA(BitmapFileName);
		if (lr==0) {
			SetColorKey(0,0);
			//	yga�̓p���b�g���܂܂Ȃ�
			m_bLoadBitmap = true;
			m_bYGA = true;
		}
		return lr;
	}
#endif

	if (m_bUseDIBSection){
		LRESULT lr = InnerLoad(BitmapFileName);
		if (lr==0) {
			SetColorKey(0,0);
/*			if (bLoadPalette) {
				if (CAppManager::GetMyDIBDraw()->GetPalette()->Get(GetDC())==0)
					CAppManager::GetMyDIBDraw()->RealizePalette();
			}*/
			m_bLoadBitmap = true;
		}
		return lr;
	}

	CDIBitmap dib;
	//	32Bpp�ō���Ă����Ȃ��ƃR�s�[�������[�ōς܂Ȃ�
	if (dib.Load(BitmapFileName,32)!=0) return 1;

	LONG x,y;
	if (dib.GetSize(x,y)!=0) return 2;

	InnerCreateSurface(x,y);

	HDC hdc = dib.GetDC();
	if (hdc==NULL) return 3;

	//	�p���b�g���ǂݍ��݂��Ƃ��`���ƁO�O�G
/*	if (bLoadPalette) {
		if (CAppManager::GetMyDIBDraw()->GetPalette()->Get(GetDC())==0)
			CAppManager::GetMyDIBDraw()->RealizePalette();
	}*/

	DWORD* lpdw = GetPtr();
	/*
	for(int j=0;j<y;j++){
		for(int i=0;i<x;i++){
			COLORREF color = ::GetPixel(hdc,i,j);
			DWORD	 dwPixel;
			//	���ʂ���RGB�ɂȂ��Ă���̂�BGR�ɂ���
			_asm {
				mov	  eax,color
				bswap eax
				shr	  eax,8
				mov	  dwPixel,eax
			}
			*(lpdw++) = dwPixel;
			//*(lpdw++) = color;
		}
	}
	*/
	DWORD*	lpdwSrc = (DWORD*)dib.GetPtr();
	::CopyMemory(lpdw,lpdwSrc,x*y << 2);

	//	���ƁA����ł͍ŏ�ʃo�C�g���s��B�Ƃقف`���ˁO�O�G
	ClearMSByte();

	m_bLoadBitmap = true;
	SetColorKey(0,0);

	return 0;
}

LRESULT CDIB32Base::InnerLoad(string BitmapFileName){
	//	DIBSection���g���Ă���Ƃ��p
	//	CDIBitmap����p�N���O�O
	CFile file;
	file.Read(BitmapFileName);

	auto_ptrEx<CGraphicLoader> gl(CGraphicLoader::GetPrototypeFactory()->CreateInstance());
	if (gl->LoadPicture(file)) return 1;

	LONG x,y;
	if (gl->GetSize(x,y)) return 2;
	if (InnerCreateSurface(x,y)) return 3;
	if (gl->Render(GetPtr(),/* GetPitch()*/ GetRect()->right * 4)==0) return 0;
//		//	���̃T�[�t�F�[�X�̃|�C���^�n���ă����_�����O�ł���΁A����ł��n��

	if (gl->Render(GetDC())) return 4;

	//	���ƁA����ł͍ŏ�ʃo�C�g���s��B�Ƃقف`���ˁO�O�G
	ClearMSByte();

	return 0;
}

void	CDIB32Base::ClearMSByte() {
	//	���ƁA����ł͍ŏ�ʃo�C�g���s��B�Ƃقف`���ˁO�O�G
	DWORD *p = GetPtr();
	int len = GetRect()->right * GetRect()->bottom;
	while( len-- ) *p++ &= 0x00FFFFFF;
}

#ifdef USE_YGA
LRESULT	CDIB32Base::InnerLoadYGA(string BitmapFileName){
	CFile file;
	if (file.Read(BitmapFileName)!=0) return 1;

	BYTE* lpSrc = (BYTE*)file.GetMemory();
	SYPGHeader& header = *(SYPGHeader*)lpSrc;

	if (InnerCreateSurface(header.dwSizeX,header.dwSizeY)!=0) return 2;
	if (!header.bCompress) {
		//	�񈳏k�]��
		::CopyMemory(GetPtr(),lpSrc+sizeof(SYPGHeader),header.dwOriginalSize);
	} else {
		// ���k����Ă���̂ŉ𓀂��Ȃ���̓]��
		CLZSS lzss;
		BYTE* lpDst = (BYTE*)GetPtr();
		if (lzss.Decode(lpSrc+sizeof(SYPGHeader),lpDst
			,header.dwOriginalSize,false)!=0) return 3;
	}
	return 0;
};
#endif

LRESULT CDIB32Base::Save(string BitmapFileName,LPRECT lpRect){
	//	�t�@�C���Ƃ��ď����o��
	if (m_lpdwSrc == NULL) return 0;

	if (lpRect==NULL) lpRect = &m_rcRect;

	//	���l�͖��g�p�Ȃ̂�32bpp�ł͂Ȃ�24bpp�ŏ\���I

	int sx = lpRect->right	- lpRect->left;
	int sy = lpRect->bottom - lpRect->top;

	CDIBitmap dib;
	dib.CreateSurface(sx,sy);

	HDC hdc = dib.GetDC();
	if (hdc==NULL) return 1;

	HDC hdcsrc = GetDC();
	if (hdcsrc!=NULL) {
		//	BitBlt�g����Ƃ��͎g�����ق��������悤�ȋC������O�O
		if (!::BitBlt(hdc,0,0,sx,sy
		,hdcsrc,lpRect->left,lpRect->top,SRCCOPY))
			return 2;
	} else {
		//	�C���[�W�]���Ȃ��I
		for(int j=0,y=lpRect->top;y<lpRect->bottom;y++,j++){
			for(int i=0,x=lpRect->left;x<lpRect->right;x++,i++){
				DWORD dwPixel = GetPixel(x,y);
				//	�ۑ���˂񂩂�A�܂��A������Ƃ��炢�x���Ă��������ȁH
				//	���ʂ���RGB�ɂȂ��Ă���̂�BGR�ɂ���
				_asm {
					mov		eax,dwPixel
					bswap	eax
					shr		eax,8
					mov		dwPixel,eax
				}
				::SetPixel(hdc,i,j,dwPixel);
			}
		}
	}
	LRESULT lr = dib.Save(BitmapFileName);

	return lr;
}

#ifdef USE_YGA
LRESULT CDIB32Base::SaveYGA(string BitmapFileName,LPRECT lpRect, bool bCompress ){

	if (lpRect!=NULL) return 1; // ���T�|�[�g(�߁X�T�|�[�g����)

	//	���k���|���āA�w�b�_�[����t�^����
	CLZSS lzss; BYTE* lpDst; DWORD dwDstSize; bool bDelete;
	DWORD nSize = GetRect()->right * GetRect()->bottom * 4;
	if (! bCompress || lzss.Encode((BYTE*)GetPtr(),lpDst,nSize,dwDstSize)!=0 ) {
		// ���k���Ȃ� or ���k��������[��
		lpDst = (BYTE*)GetPtr();
		dwDstSize = nSize;
		bDelete = false;
	} else {
		bDelete = true;
	}

	//	�w�b�_�[�̐ݒ�
	SYPGHeader header;
	header.dwSizeX = GetRect()->right;
	header.dwSizeY = GetRect()->bottom;
	header.bCompress = (DWORD) bDelete;
	header.dwOriginalSize  = nSize;
	header.dwCompressSize = dwDstSize;

	nSize = sizeof(SYPGHeader) + dwDstSize;
	auto_array<BYTE> writebuf(nSize);
	::CopyMemory(writebuf,&header,sizeof(SYPGHeader));
	::CopyMemory(writebuf+sizeof(SYPGHeader),lpDst,dwDstSize);	
	if (bDelete) DELETEPTR_SAFE(lpDst);

	CFile f;
	return f.Write(BitmapFileName,(void*)(BYTE*)writebuf,nSize);
}
#endif

LRESULT CDIB32Base::Release(){
	if (m_hDC!=NULL){
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}
	if (m_hBitmap!=NULL){
		::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
		m_lpdwSrcOrg = NULL;
		m_lpdwSrc = NULL;
	} else {
		DELETEPTR_SAFE(m_lpdwSrcOrg);
		m_lpdwSrc = NULL;
	}
	m_lpdwLineStart.clear();
	m_bLoadBitmap = false;
	m_bYGA = false;

	ZERO(m_rcRect);
	return 0;
}

bool	CDIB32Base::IsLoad(void) const {
	return m_bLoadBitmap;
}

void CDIB32Base::SetFillColor(DWORD dwDIB32RGB) {
	m_dwFillColor = dwDIB32RGB;
}

LRESULT CDIB32Base::Clear(DWORD dwDIB32RGB,LPRECT lpRect){
	if (m_lpdwSrc == NULL) return 1;	//	�Ȃ��H�m�ۂ���Ƃ���^^

	if (dwDIB32RGB == CLR_INVALID) {
		dwDIB32RGB = m_dwFillColor;
	}
	
	if (lpRect==NULL) {
	//	��ʑS�̂ɑ΂���I�y���[�V����������...
		LONG lSize = m_rcRect.right * m_rcRect.bottom;
		DWORD* lpdwSrc = m_lpdwSrc;
		_asm {
			cld		//	for stosd ahead
			mov		ecx,lSize
			mov		eax,dwDIB32RGB
			mov		edi,lpdwSrc
			rep		stosd
		}
	} else {
		//	��`�ɑ΂���I�y���[�V����������...
		int sx = lpRect->left, ex = lpRect->right,
			sy = lpRect->top , ey = lpRect->bottom;

		//	�ꉞ�A�͈͊O���ǂ����`�F�b�N���Ƃ���...
		WARNING(sx<m_rcRect.left || ex>m_rcRect.right
			||	sy<m_rcRect.top	 || ey>m_rcRect.bottom ,
			"CDIB32::Clear�̋�`���͈͊O");

		BYTE* lpby = (BYTE*)(m_lpdwSrc + sx + sy*m_rcRect.right);
		LONG lSize	= ex-sx;

		for(int y=sy;y<ey;y++) {
			_asm {
				//	for����direction flag�Ɋ֗^���Ȃ����낤���A
				//	�ꉞ�A���[�v���ɐݒ肵�Ă�����...
				cld		//	for stosd ahead
				mov		ecx,lSize
				mov		eax,dwDIB32RGB
				mov		edi,lpby
				rep		stosd
			}
			lpby += m_lPitch;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

// ���߃L�[�ݒ�֘A
LRESULT CDIB32Base::SetColorKey(DWORD dwDIB32RGB){
	// ����̐F��]���̂Ƃ��̓��߃L�[�ɐݒ肷��
	m_dwColorKey = dwDIB32RGB & 0xffffff;		//	�O�̂��߃}�X�N�|���Ă����B
	return 0;	//	���s�͂��肦�Ȃ�
}

LRESULT CDIB32Base::SetColorKey(int r,int g,int b){
	m_dwColorKey = DIB32RGB(r,g,b) & 0xffffff;
	return 0;
}

LRESULT CDIB32Base::SetColorKey(int x,int y){		// (x,y)�̓_�𓧉߃L�[�ɐݒ肷��
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::SetColorKey��m_lpdwSrc == NULL");
	if (x<m_rcRect.left || x>=m_rcRect.right
		 || y<m_rcRect.top || y>=m_rcRect.bottom)
		return 1;	//	�͈̓I�[�o�[���Ă���

	m_dwColorKey = GetPixel(x,y) & 0xffffff;	//	�O�̂��߃}�X�N�|���Ă����B
	return 0;
}

DWORD	CDIB32Base::GetColorKey(void){
	return m_dwColorKey;
}

///////////////////////////////////////////////////////////////////////////////
//	�s�N�Z������n

int	CDIB32Base::GetPixelAlpha(int x,int y){
	if (IsYGA()) {
		return GetPixel(x,y) >> 24;
	} else {
		//	�͈͊O�Ȃ�� 0 �ƂȂ݂��B
		if (x<0 || y<0 || x>=m_rcRect.right || y>=m_rcRect.bottom) return 0;

		DWORD dw = GetPixel(x,y);
		//	���ꂪ�����F�Ȃ�΁A0�B�񔲂��F�Ȃ��255�Ƃ݂Ȃ�
		if (GetColorKey() == dw) {
			return 0;
		}
		return 255;
	}
}


DWORD	CDIB32Base::GetPixel(int x,int y) {
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::GetPixel��m_lpdwSrc == NULL");
	//	���S�̂��߁O�O�G
	if (x<0 || y<0 || x>=m_rcRect.right || y>=m_rcRect.bottom) return 0;

	return *(m_lpdwSrc+x+y*m_rcRect.right);
}
void	CDIB32Base::SetPixel(int x,int y,DWORD dwDIB32RGB) {
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::SetPixel��m_lpdwSrc == NULL");
	//	���S�̂��߁O�O�G
	if (x<0 || y<0 || x>=m_rcRect.right || y>=m_rcRect.bottom) return ;

	*(m_lpdwSrc+x+y*m_rcRect.right)= dwDIB32RGB;
}
void	CDIB32Base::GetSize(int& sx,int& sy) {
	//	WARNING(m_lpdwSrc == NULL,"CDIB32Base::GetSize��m_lpdwSrc == NULL");

	//	�ǂݍ���ł��Ȃ��Ƃ���(0,0)���Ԃ�悤�ɕύX
	if (m_lpdwSrc == NULL) {
		sx = sy = 0;
	} else {
		sx = m_rcRect.right;
		sy = m_rcRect.bottom;
	}
}
DWORD*	CDIB32Base::GetPtr() {
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::GetPtr��m_lpdwSrc == NULL");
	return m_lpdwSrc;
}

///////////////////////////////////////////////////////////////////////////////

CDIB32Base::CDIB32Base(void) {
	m_lpdwSrcOrg = NULL;	//	�������͊m�ۂ��Ƃ�����ˁ[�O�O
	m_lpdwSrc	 = NULL;
	m_hBitmap	 = NULL;
	m_bUseDIBSection = false;
	m_hDC = NULL;			//	������Y��Ă����... ('00/09/19)
	m_dwColorKey = 0;		//	�Y��Ă���O�O�G
	m_bLoadBitmap = false;
	m_dwFillColor = 0;		//	Clear�F
	m_bYGA	= false;		//	�f�B�t�H���g��false
	ZERO(m_rcRect);			//	�T�C�Y�͂��ꂩ�犷�Z�����ŏ����l�͂O
}

CDIB32Base::~CDIB32Base(){
	Release();
}

///////////////////////////////////////////////////////////////////////////////

LRESULT CDIB32Base::Clipping(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();

	//	�\�[�X��`��NULL�Ȃ�΁A�S��
	if (lpSrcRect==NULL) {
		m_rcSrcRect = p->m_rcRect;
	} else {
		m_rcSrcRect = *lpSrcRect;
	}

	//	�N���b�v�̈�
	LPRECT lpClip;
	if (lpClipRect == NULL){
		lpClip = &m_rcRect;
	} else {
		lpClip = lpClipRect;

//		WARNING ( lpClip->left	< m_rcRect.left	|| lpClip->top	  < m_rcRect.top
//			||	  lpClip->right > m_rcRect.right || lpClip->bottom > m_rcRect.bottom,
//			"CDib32Base::Clipping�ŃN���b�v��`���]����T�[�t�F�[�X����͂ݏo���Ă���");
	}

	//--- �ǉ� '02/03/04  by ENRA ---
	{	// �N���b�vRect�́A�]����Rect�ɓ����Ȃ��Ƃ����Ȃ�
		// ������ClipRect���I�͂���ȏ��ɂ������ꍇ�̏����͂ǂ����悤�c
		LPRECT lpDstRect2 = &m_rcRect;		// �]����͎���
		int t;
		//--- �C�� '02/04/08  by ENRA ---
		// �����0�ɂ����炠���񂩂���^^;;
		t = lpClip->left  - lpDstRect2->left;
		if (t<0)	{ lpClip->left  = lpDstRect2->left; }
		t = lpClip->top - lpDstRect2->top;
		if (t<0)	{ lpClip->top = lpDstRect2->top; }
		//-------------------------------
		t = lpClip->right  - lpDstRect2->right;
		if (t>0)	{ lpClip->right  = lpDstRect2->right; }
		t = lpClip->bottom - lpDstRect2->bottom;
		if (t>0)	{ lpClip->bottom = lpDstRect2->bottom; }
	}
	//-------------------------------

	if ((lpDstSize==NULL)  ||
		((lpDstSize->cx == (m_rcSrcRect.right - m_rcSrcRect.left)) &&
		 (lpDstSize->cy == (m_rcSrcRect.bottom - m_rcSrcRect.top)))) {
		//	���{�̓]���H
		m_bActualSize = true;

		// �N���b�s���O����
		// this clipping algorithm is thought by yaneurao(M.Isozaki)
		int sx = m_rcSrcRect.right - m_rcSrcRect.left;
		int sy = m_rcSrcRect.bottom - m_rcSrcRect.top;
		::SetRect(&m_rcDstRect,x,y,x+sx,y+sy);

		int t;
		t = lpClip->left-x;
		if (t>0)	{ m_rcSrcRect.left += t;	m_rcDstRect.left = lpClip->left; }
		t = lpClip->top -y;
		if (t>0)	{ m_rcSrcRect.top  += t;	m_rcDstRect.top	 = lpClip->top;	 }
		t = x+sx-lpClip->right;
		if (t>0)	{ m_rcSrcRect.right -= t;	m_rcDstRect.right = lpClip->right; }
		t = y+sy-lpClip->bottom;
		if (t>0)	{ m_rcSrcRect.bottom -= t;	m_rcDstRect.bottom = lpClip->bottom; }

		//	invalid rect ?
		if (m_rcSrcRect.left >= m_rcSrcRect.right ||
			m_rcSrcRect.top	 >= m_rcSrcRect.bottom) return 1;

	} else {
		//	�񓙔{�̓]���H

		m_bActualSize = false;
		::SetRect(&m_rcDstRect,x,y,x+lpDstSize->cx,y+lpDstSize->cy);

		//	�u���[���n���̏����l���v�Z����

		//	Initial(x,y) = -Dst size(x,y)
		m_nInitialX = m_rcDstRect.left - m_rcDstRect.right;
		m_nInitialY = m_rcDstRect.top  - m_rcDstRect.bottom;

		m_nStepX	= (m_rcSrcRect.right  - m_rcSrcRect.left) ;
		m_nStepY	= (m_rcSrcRect.bottom - m_rcSrcRect.top ) ;

		m_nCmpX = - (m_nInitialX);
		m_nCmpY = - (m_nInitialY);

		// invalid rect
		if (m_nCmpX == 0 || m_nCmpY == 0) return 1;

		// �N���b�s���O����
		// this clipping algorithm is thought by yaneurao(M.Isozaki)

		//	�~���[���Ɣ�~���[���Ńu���[���n���̏����l���Ⴄ
		int t;
		t = lpClip->left-m_rcDstRect.left;
		if (t>0)	{
			m_nInitialX+=t*m_nStepX;	//	���ۂɃu���[���n�����Ă݂�
			if (m_nInitialX > 0){
				int s = m_nInitialX / m_nCmpX + 1;
				m_rcSrcRect.left += s;	//	not mirror!
				m_nInitialX		-= s*m_nCmpX;
			}
			m_rcDstRect.left   = lpClip->left;
		}
		t = lpClip->top -m_rcDstRect.top;
		if (t>0)	{
			m_nInitialY+=t*m_nStepY;	//	���ۂɃu���[���n�����Ă݂�
			if (m_nInitialY > 0){
				int s = m_nInitialY / m_nCmpY +1;
				m_rcSrcRect.top += s;
				m_nInitialY		-= s*m_nCmpY;
			}
			m_rcDstRect.top = lpClip->top;
		}
		t = m_rcDstRect.right-lpClip->right;
		if (t>0)	{
			m_rcDstRect.right  = lpClip->right;
/*
			int nInitialX = m_nInitialX;
			nInitialX+=(m_rcDstRect.right-m_rcDstRect.left)*m_nStepX;	//	���ۂɃu���[���n�����Ă݂�
			if (nInitialX > 0){
				int s = nInitialX / m_nCmpX + 1;
				m_rcSrcRect.right = m_rcSrcRect.left + s;
			}
		} else {
			m_rcSrcRect.right--;
*/		}
		//	srcRect�̎Z�o�Bdst�̂͂ݏo���������T����̂����At*m_nStepX/m_nCmp��roundup����K�v����B

		t = m_rcDstRect.bottom-lpClip->bottom;
		if (t>0)	{ /*m_rcSrcRect.bottom -= t;*/	m_rcDstRect.bottom = lpClip->bottom;}

		//	invalid rect ?
		if (m_rcSrcRect.left >= m_rcSrcRect.right ||
			m_rcSrcRect.top	 >= m_rcSrcRect.bottom ||
			m_rcDstRect.left >= m_rcDstRect.right ||
			m_rcDstRect.top	 >= m_rcDstRect.bottom) return 1;

		//	m_nStepX < m_nCmpX��ۏ؂���B
		m_nStepsX = m_nStepX/m_nCmpX;
		m_nStepX -= m_nCmpX*m_nStepsX;

		m_nStepsY = m_nStepY/m_nCmpY;
		m_nStepY -= m_nCmpY*m_nStepsY;

		// �ǂ��łP�������̂�������Ȃ��̂ŁA�����ň����̂��� By Tia
		m_nInitialX--;
		m_nInitialY--;
	}

	WARNING ( m_rcSrcRect.left	< p->m_rcRect.left	|| m_rcSrcRect.top	   < p->m_rcRect.top
		||	  m_rcSrcRect.right > p->m_rcRect.right || m_rcSrcRect.bottom  > p->m_rcRect.bottom,
		"CDib32Base::Clipping�œ]�����T�[�t�F�[�X�̎w�肪�T�[�t�F�[�X����͂ݏo���Ă���");

	return 0;
}

LRESULT CDIB32Base::ClippingM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();

	//	�\�[�X��`��NULL�Ȃ�΁A�S��
	if (lpSrcRect==NULL) {
		m_rcSrcRect = p->m_rcRect;
	} else {
		m_rcSrcRect = *lpSrcRect;
		if (m_rcSrcRect.left > m_rcSrcRect.right) {
			return 1;	//	invalid rect
		}
	}

	//	�N���b�v�̈�
	LPRECT lpClip;
	if (lpClipRect == NULL){
		lpClip = &m_rcRect;
	} else {
		lpClip = lpClipRect;

//		WARNING ( lpClip->left	< m_rcRect.left	 || lpClip->top	   < m_rcRect.top
//			||	  lpClip->right > m_rcRect.right || lpClip->bottom > m_rcRect.bottom,
//			"CDib32Base::Clipping�ŃN���b�v��`���]����T�[�t�F�[�X����͂ݏo���Ă���");
	}

	//--- �ǉ� '02/03/04  by ENRA ---
	{	// �N���b�vRect�́A�]����Rect�ɓ����Ȃ��Ƃ����Ȃ�
		// ������ClipRect���I�͂���ȏ��ɂ������ꍇ�̏����͂ǂ����悤�c
		LPRECT lpDstRect2 = &m_rcRect;		// �]����͎���
		int t;
		//--- �C�� '02/04/08  by ENRA ---
		// �����0�ɂ����炠���񂩂���^^;;
		t = lpClip->left  - lpDstRect2->left;
		if (t<0)	{ lpClip->left  = lpDstRect2->left; }
		t = lpClip->top - lpDstRect2->top;
		if (t<0)	{ lpClip->top = lpDstRect2->top; }
		//-------------------------------
		t = lpClip->right  - lpDstRect2->right;
		if (t>0)	{ lpClip->right  = lpDstRect2->right; }
		t = lpClip->bottom - lpDstRect2->bottom;
		if (t>0)	{ lpClip->bottom = lpDstRect2->bottom; }
	}
	//-------------------------------


	if ((lpDstSize==NULL)  ||
		((lpDstSize->cx == (m_rcSrcRect.right - m_rcSrcRect.left)) &&
		 (lpDstSize->cy == (m_rcSrcRect.bottom - m_rcSrcRect.top)))) {
		//	���{�̓]���H
		m_bActualSize = true;

		// �N���b�s���O����
		// this clipping algorithm is thought by yaneurao(M.Isozaki)
		int sx = m_rcSrcRect.right - m_rcSrcRect.left;
		int sy = m_rcSrcRect.bottom - m_rcSrcRect.top;
		::SetRect(&m_rcDstRect,x,y,x+sx,y+sy);

		int t;
		t = lpClip->left-x;
		if (t>0)	{ m_rcSrcRect.right -= t;	m_rcDstRect.left = lpClip->left; }
		t = lpClip->top -y;
		if (t>0)	{ m_rcSrcRect.top  += t;	m_rcDstRect.top	 = lpClip->top;	 }
		t = x+sx-lpClip->right;
		if (t>0)	{ m_rcSrcRect.left += t;	m_rcDstRect.right = lpClip->right; }
		t = y+sy-lpClip->bottom;
		if (t>0)	{ m_rcSrcRect.bottom -= t;	m_rcDstRect.bottom = lpClip->bottom; }

		//	invalid rect ?
		if (m_rcSrcRect.left >= m_rcSrcRect.right ||
			m_rcSrcRect.top	 >= m_rcSrcRect.bottom) return 1;

	} else {
		//	�񓙔{�̓]���H

		m_bActualSize = false;
		::SetRect(&m_rcDstRect,x,y,x+lpDstSize->cx,y+lpDstSize->cy);

		//	�u���[���n���̏����l���v�Z����

		//	Initial(x,y) = -Dst size(x,y)
		m_nInitialX = m_rcDstRect.left - m_rcDstRect.right;
		m_nInitialY = m_rcDstRect.top  - m_rcDstRect.bottom;

		m_nStepX	= (m_rcSrcRect.right  - m_rcSrcRect.left) << 1;
		m_nStepY	= (m_rcSrcRect.bottom - m_rcSrcRect.top ) << 1;

		m_nCmpX = - (m_nInitialX << 1);
		m_nCmpY = - (m_nInitialY << 1);

		// invalid rect
		if (m_nCmpX == 0 || m_nCmpY == 0) return 1;

		// �N���b�s���O����
		// this clipping algorithm is thought by yaneurao(M.Isozaki)

		//	�~���[���Ɣ�~���[���Ńu���[���n���̏����l���Ⴄ
		int t;
		t = lpClip->left-m_rcDstRect.left;
		if (t>0)	{
			m_nInitialX+=t*m_nStepX;	//	���ۂɃu���[���n�����Ă݂�
			if (m_nInitialX > 0){
				int s = m_nInitialX / m_nCmpX +1;
				//	�~���[�̂Ƃ��́ASrcRect��right����X�L�������Ă�
				m_rcSrcRect.right -= s; // mirror!
//				m_rcSrcRect.left += s;	// mirror!
				m_nInitialX		-= s*m_nCmpX;
			}
			m_rcDstRect.left   = lpClip->left;
		}
		t = lpClip->top -m_rcDstRect.top;
		if (t>0)	{
			m_nInitialY+=t*m_nStepY;	//	���ۂɃu���[���n�����Ă݂�
			if (m_nInitialY > 0){
				int s = m_nInitialY / m_nCmpY +1;
				m_rcSrcRect.top += s;
				m_nInitialY		-= s*m_nCmpY;
			}
			m_rcDstRect.top = lpClip->top;
		}
		t = m_rcDstRect.right-lpClip->right;
		if (t>0)	{
			m_rcDstRect.right  = lpClip->right;
/*
			int nInitialX = m_nInitialX;
			nInitialX+=(m_rcDstRect.right-m_rcDstRect.left)*m_nStepX;	//	���ۂɃu���[���n�����Ă݂�
			if (nInitialX > 0){
				int s = nInitialX / m_nCmpX + 1;
				m_rcSrcRect.left = m_rcSrcRect.right - s;
			}
*/		}

		t = m_rcDstRect.bottom-lpClip->bottom;
		if (t>0)	{ /*m_rcSrcRect.bottom -= t;*/	m_rcDstRect.bottom = lpClip->bottom;}

		//	invalid rect ?
		if (m_rcSrcRect.left >= m_rcSrcRect.right ||
			m_rcSrcRect.top	 >= m_rcSrcRect.bottom ||
			m_rcDstRect.left >= m_rcDstRect.right ||
			m_rcDstRect.top	 >= m_rcDstRect.bottom) return 1;

		//	m_nStepX < m_nCmpX��ۏ؂���B
		m_nStepsX = m_nStepX/m_nCmpX;
		m_nStepX -= m_nCmpX*m_nStepsX;

		m_nStepsY = m_nStepY/m_nCmpY;
		m_nStepY -= m_nCmpY*m_nStepsY;

		// �ǂ��łP�������̂�������Ȃ��̂ŁA�����ň����̂��� By Tia
		m_nInitialX--;
		m_nInitialY--;
	}

	WARNING ( m_rcSrcRect.left	< p->m_rcRect.left	|| m_rcSrcRect.top	  < p->m_rcRect.top
		||	  m_rcSrcRect.right > p->m_rcRect.right || m_rcSrcRect.bottom > p->m_rcRect.bottom,
		"CDib32Base::Clipping�œ]�����T�[�t�F�[�X�̎w�肪�T�[�t�F�[�X����͂ݏo���Ă���");

	return 0;
}

LRESULT CDIB32Base::BltNatural(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	//	����́A�蓮�|�����[�t�B�Y���̂��߂̎d�|���ł���O�O�G
	if (!lpDIBSrc32->IsYGA()) {
		//	�ʏ�]��
		return Blt(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	} else {
		//	�u�����h�]��
		return BlendBltFastAlpha(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}
}

LRESULT CDIB32Base::BltNatural(CDIB32* lpDIBSrc32,int x,int y,int nFade,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect){
	//	����́A�蓮�|�����[�t�B�Y���̂��߂̎d�|���ł���O�O�G
	if (nFade==0) return 0;
	//	�ʏ�\�����D�D
	if (nFade==256) {
		return BltNatural(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}
	
	DWORD dw = PlaneRGB(nFade,nFade,nFade);
	if (!lpDIBSrc32->IsYGA()) {
		//	�ʏ�]��
		return BlendBlt(lpDIBSrc32,x,y,0xffffff-dw,dw,lpSrcRect,lpDstSize,lpClipRect);
	} else {
		//	�u�����h�]��
		return BlendBltFastAlpha(lpDIBSrc32,x,y,0xffffff-dw,dw,lpSrcRect,lpDstSize,lpClipRect);
	}
}
///////////////////////////////////////////////////////////////////////////////

LRESULT CDIB32P5::ShadeOff(LPRECT lpSrcRect){

	//	easy clipping
	RECT rc;
	if (lpSrcRect == NULL){
		rc = m_rcRect;
	} else {
		rc = *lpSrcRect;
	}
	if (rc.left	  < m_rcRect.left)	{ rc.left	= m_rcRect.left; }
	if (rc.top	  < m_rcRect.top)	{ rc.top	= m_rcRect.top;	}
	if (rc.right  > m_rcRect.right)	{ rc.right	= m_rcRect.right;}
	if (rc.bottom > m_rcRect.bottom){ rc.bottom = m_rcRect.bottom;}

	if (rc.left >= rc.right || rc.top >= rc.bottom) return 1;	//	invalid rect

	int sx = rc.right  - rc.left;
	int sy = rc.bottom - rc.top;

	if (sx<=3 || sy<=3) return 2; // too small area..

	//	�������v���b�g�����_���x����Ă͂����Ȃ��B
	//	���̂��߁A�����ł͒x���������݂��s�Ȃ��B
	auto_array<DWORD> lpDst(sx);
	DWORD pixel;
	int i,y;
	for (y = rc.top ; y < rc.bottom-1 ; y++ ){
		DWORD* lpSrc	= m_lpdwSrc+(rc.left  )+(y	)*m_rcRect.right;
		DWORD* lpUp		= m_lpdwSrc+(rc.left  )+(y-1)*m_rcRect.right;
		DWORD* lpDown	= m_lpdwSrc+(rc.left  )+(y+1)*m_rcRect.right;
		DWORD* lpLeft	= m_lpdwSrc+(rc.left-1)+(y	)*m_rcRect.right;
		DWORD* lpRight	= m_lpdwSrc+(rc.left+1)+(y	)*m_rcRect.right;

		if (y==rc.top){
			//	��[
			::CopyMemory(lpDst,lpSrc,sx*sizeof(DWORD));
			continue;
		}

		//	���[
		pixel = (*lpUp&0xfff8f8f8) + (*(lpDown++)&0xfff8f8f8) + ((*(lpRight++)&0xfff8f8f8)<<1) + ((*(lpSrc++)&0xfff8f8f8)<<2);
		*(lpUp++) = lpDst[0];		//	�x����������
		lpLeft++;
		lpDst[0] = pixel >> 3;
		//	����
		for (i = 1 ; i < sx-1 ; i++ ){
			pixel = (*lpUp&0xfff8f8f8) + (*(lpDown++)&0xfff8f8f8) + (*(lpLeft++)&0xfff8f8f8) + (*(lpRight++)&0xfff8f8f8) + ((*(lpSrc++)&0xfff8f8f8)<<2);
			*(lpUp++) = lpDst[i];		//	�x����������
			lpDst[i] = pixel >> 3;
		}
		//	�E�[
		pixel = (*lpUp&0xfff8f8f8) + (*(lpDown++)&0xfff8f8f8) + ((*(lpLeft++)&0xfff8f8f8)<<1) + ((*(lpSrc++)&0xfff8f8f8)<<2);
		lpRight++;
		*(lpUp++) = lpDst[i];		//	�x����������
		lpDst[i] = pixel >> 3;

	}
	DWORD* lpSrc	= m_lpdwSrc+(rc.left  )+( y-1 )*m_rcRect.right;
	::CopyMemory(lpSrc,lpDst,sx*sizeof(DWORD));					//	�x���o�b�t�@�̃������C��
	::CopyMemory(lpSrc+m_rcRect.right,lpDst,sx*sizeof(DWORD));	//	�Ō�̃������C���͂��܂����O�O�G
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

LRESULT CDIB32P5::MosaicEffect(int d, LPRECT lpRect){
	//�@�v���[���ɑ΂��ă��U�C�N��������@�\		added '99/12/1
	//	d :	�ʎq�����x��
	if (d==0) return -2;	//	����ŗ�����̂͂܂����낤...

	RECT r = GetClipRect(lpRect);
	LONG lPitch	 = GetRect()->right;
	DWORD* pSurface = GetPtr();

	for(int y=r.top;y<r.bottom;y+=d){
		int d2;		//	���[�̒[��
		if (y+d>r.bottom) d2=r.bottom-y; else d2=d;
		for(int x=r.left;x<r.right;x+=d){
			int d1;	//	�E�[�̒[��
			if (x+d>r.right) d1=r.right-x; else d1=d;
			
			DWORD *p,*p2;
			p = pSurface + y*lPitch + x;
			DWORD c;	// ��\�_�̐F
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
	return 0;
}

LRESULT CDIB32P5::FlushEffect(LPRECT lpRect){
	//	�v���[���ɑ΂��ăl�K�|�W���]������@�\		added '99/12/1

	RECT r = GetClipRect(lpRect);
	LONG lPitch	 = GetRect()->right;
	DWORD* pSurface = GetPtr();

	for(int y=r.top;y<r.bottom;y++){
		DWORD *p = pSurface + y*lPitch + r.left;
		for(int x=r.left;x<r.right;x++){
			*(p++) ^= 0xffffff;	//	����24�r�b�g�ɑ΂���xor���邾��:p
		}
	}

	return 0;
}

LRESULT CDIB32P5::FadeEffect(int nFade,LPRECT lpRect){
	//	�v���[���̃u���C�g�l�X�𗎂Ƃ��@�\	added '00/10/03
	//	�蔲�������ǁA����Ȃ�ɑ����̂ŁA������[�O�O�G

	RECT r = GetClipRect(lpRect);
	LONG lPitch	 = GetRect()->right;
	DWORD* pSurface = GetPtr();

	static DWORD dwTable[256];
	static int nSelectTable = -1;
	if (nSelectTable != nFade){
	//	�e�[�u���̍��Ȃ���
		DWORD dwCount = 0;
		for(int i=0;i<256;++i,dwCount+=nFade)
			dwTable[i] = dwCount >> 8;
		nSelectTable = nFade;
	}

	for(int y=r.top;y<r.bottom;y++){
		DWORD *p = pSurface + y*lPitch + r.left;
		for(int x=r.left;x<r.right;x++){
			DWORD dwPixel = *p;
			dwPixel =  (dwTable[dwPixel & 0xff	  ])
					+  (dwTable[(dwPixel>>8)&0xff ]<<8 )
					+  (dwTable[(dwPixel>>16)&0xff]<<16);
			*(p++) = dwPixel;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//	�eCPU���Ƃ̃��[�`��(assisted by Tia Deen)

////////////////////////////////////////////////////////////////////////////
//	Pentium�p�̃��[�`��
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//	�~���[������`�]��
////////////////////////////////////////////////////////////////////
//////////////////////////////////////
//	Blt
//	�����F�L��̓]��
//////////////////////////////////////
LRESULT CDIB32P5::Blt(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::Blt��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::Blt��p->GetPtr() == NULL");

// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;
	DWORD	lPitchSrc = p->m_lPitch;
	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth =	  m_lPitch - (nWidth<<2);									// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
		DWORD		colKey = p->m_dwColorKey;


		_asm
		{
			MOV		ECX, nWidth
			MOV		EDX, nHeight

			MOV		EDI, lpDst
			MOV		ESI, lpSrc

			MOV		EBX, colKey				// UnPair

		LoopX_1:	// 5(4)�N���b�N��T�C�N��
			MOV		EAX, [ESI]
			ADD		ESI, 4

			CMP		EAX, EBX
			JE		Skip_1

			MOV		[EDI], EAX				// UnPair

		Skip_1:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		ECX

			JNZ		LoopX_1					// UnPair

			MOV		ECX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			DEC		EDX

			JNZ		LoopX_1					// UnPair
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:�L	 --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		int		i, j;
		BYTE*	lpSrcBack;

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;
		LoopY_2:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX				// UnPair			// EY = InitializeX;

		LoopX_2: // 7(6)�N���b�N��T�C�N��
			MOV		EAX, [ESI]									// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			CMP		EAX, colKey
			JE		SkipColKey_2

			MOV		[EDI], EAX				// UnPair

		SkipColKey_2:
			ADD		EDI, 4					// UnPair			// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_2										// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_2:
			DEC		i
			JNZ		LoopX_2

			MOV		ESI, lpSrcBack;								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_2					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc
			SUB		EDX, EY2				// UnPair			// Y�̕␳
			
		SkipY_2:
			DEC		j
			JNZ		LoopY_2
		}
	} // if

	
	return 0;
} // Blt


//////////////////////////////////////
//	BltFast
//	�����F�����̓]��
//////////////////////////////////////
LRESULT CDIB32P5::BltFast(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BltFast��m_lpdwSrc == NULL");
	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::BltFast��p->GetPtr() == NULL");

// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);				// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, nHeight
			MOV		ECX, nWidth

			CLD								// UnPair			// �f�B���N�V���� �N���A

		LoopX_3: // ???�N���b�N��T�C�N��
			REP		MOVSD

			MOV		ECX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			DEC		EDX

			JNZ		LoopX_3					// UnPair
		}
	} // if
// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:�L	 --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		int		i, j;
		BYTE*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;
		

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;

		LoopY_5:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX									// EY = InitializeX;

		LoopX_5: // 5�N���b�N��T�C�N��
			MOV		EAX, [ESI]									// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			MOV		[EDI], EAX
			ADD		EDI, 4										// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_5										// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_5:
			DEC		i
			JNZ		LoopX_5

			MOV		ESI, lpSrcBack								// Src�����C���̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_5					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc			// 1���C�������Z���āA���̍s��
			SUB		EDX, EY2									// Y�̕␳

		SkipY_5:
			DEC		j
			JNZ		LoopY_5
		}
	} // if

	
	return 0;
} // BltFast


//////////////////////////////////////
//	BlendBltHalf
//	�����F�L��̂P�F�P�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::BlendBltHalf(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BlendBltHalf��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::BlendBltHalf��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);			// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:��	 1:1�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	colKey = p->m_dwColorKey;
		int		i, j;


		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		EDI, lpDst
			MOV		ESI, lpSrc

			MOV		i, EAX
			MOV		j, EBX

		LoopX_6:	// 11(4)�N���b�N��T�C�N��
			MOV		EAX, [ESI]
			ADD		ESI, 4

			CMP		EAX, colKey
			JE		Skip_6

			MOV		EBX, [EDI]
			MOV		ECX, EAX

			MOV		EDX, EBX
			AND		EAX, EBX

			XOR		ECX, EDX				// UnPair

			SHR		ECX, 1
			MOV		EBX, Mask7f

			AND		ECX, EBX				// UnPair

			ADD		EAX, ECX				// UnPair

			MOV		[EDI], EAX				// UnPair

		Skip_6:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_6					// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_6
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:�L	 1:1�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	colKey = p->m_dwColorKey;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;

		LoopY_7:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX				// UnPair			// EY = InitializeX;

		LoopX_7: // 12(6)�N���b�N��T�C�N��
			MOV		EAX, [ESI]				// UnPair			// �s�N�Z���̃R�s�[

			CMP		EAX, colKey
			JE		SkipColKey_7

			MOV		EBX, [EDI]				// UnPair

			XOR		EAX, EBX				// UnPair

			SHR		EAX, 1					// UnPair

			AND		EAX, Mask7f
			AND		EBX, [ESI]

			ADD		EAX, EBX				// UnPair

			MOV		[EDI], EAX				// UnPair

		SkipColKey_7:
			ADD		ESI, AddSrcPixel							// �������̉��Z
			ADD		EDI, 4										// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_7										// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_7:
			DEC		i
			JNZ		LoopX_7

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_7					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc			// 1���C�������Z���āA���̍s��
			SUB		EDX, EY2									// Y�̕␳
		SkipY_7:
			DEC		j
			JNZ		LoopY_7
		}
	} // if

	
	return 0;
} // BlendBltHalf


//////////////////////////////////////
//	BlendBltFastHalf
//	�����F�����̂P�F�P�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::BlendBltFastHalf(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BlendBltFastHalf��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::BlendBltFastHalf��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);				// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:��	 1:1�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;


		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		EDI, lpDst
			MOV		ESI, lpSrc

			MOV		i, EAX
			MOV		j, EBX

		LoopX_8:	// 8�N���b�N��T�C�N��
			MOV		EAX, [ESI]
			MOV		EBX, [EDI]

			MOV		ECX, EAX
			AND		EAX, EBX

			XOR		ECX, EBX
			ADD		ESI, 4

			SHR		ECX, 1
			MOV		EBX, Mask7f

			AND		ECX, EBX				// UnPair

			ADD		EAX, ECX				// UnPair

			MOV		[EDI], EAX
			ADD		EDI, 4										// EDI���ɐi�߂�

			DEC		i
			JNZ		LoopX_8

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_8
		}
	}// if
// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:�L	 1:1�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;

		LoopY_9:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX				// UnPair			// EY = InitializeX;

		LoopX_9: // 10�N���b�N��T�C�N��
			MOV		EAX, [ESI]				// UnPair			// �s�N�Z���̃R�s�[
			
			MOV		EBX, [EDI]				// UnPair

			XOR		EAX, EBX				// UnPair

			SHR		EAX, 1					// UnPair

			AND		EAX, Mask7f
			AND		EBX, [ESI]

			ADD		EAX, EBX				// UnPair

			MOV		[EDI], EAX				// UnPair

			ADD		ESI, AddSrcPixel							// �������̉��Z
			ADD		EDI, 4										// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_9										// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_9:
			DEC		i
			JNZ		LoopX_9

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z
		
			JNB		SkipY_9					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc			// 1���C�������Z���āA���̍s��
			SUB		EDX, EY2									// Y�̕␳

		SkipY_9:
			DEC		j
			JNZ		LoopY_9
		}
	} // if

	
	return 0;
} // BlendBltFastHalf


//////////////////////////////////////
//	BlendBlt
//	�����F�L���RGB�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::BlendBlt(CDIB32* lpDIB32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
						   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BlendBlt��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::BlendBlt��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);			// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:��	 RGB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EAX, dwSrcRGBRate
			MOV		ECX, dwDstRGBRate

			MOV		EBX, EAX
			MOV		EDX, ECX

			ROR		EAX, 8										// ���q�f�a���a���q�f�ɂ���
			AND		EBX, 0x00ff0000								// src:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			AND		EAX, 0xff0000ff								// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����
			AND		ECX, 0x00ff0000								// dst:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16
			MOV		bgRateSrc, EAX

			SHL		ECX, 8
			MOV		EAX, EDX

			ROR		EAX, 8
			OR		EBX, ECX									// d�q�O�Os�q�ɂȂ���

			AND		EAX, 0xff0000ff
			MOV		bgRateDst, EAX

			MOV		rRate, EBX				// UnPair

		LoopX_10: // 35�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������59�N���b�N)
			MOV		EAX, [ESI]
			ADD		ESI, 4

			CMP		EAX, colKey
			JE		SkipColKey_10

			MOV		ECX, [EDI]				// 2
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0xff000000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			SHR		EDX, 8					// 3
			MOV		SrcBackup, EAX			// 1

			MOV		ECX, EDX				// 3
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		EAX, EBX				// 2// UnPair

			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� byTia

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

		SkipColKey_10:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_10				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_10
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:�L	 RGB�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;


		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EAX, dwSrcRGBRate
			MOV		ECX, dwDstRGBRate

			MOV		EBX, EAX
			MOV		EDX, ECX

			ROR		EAX, 8										// ���q�f�a���a���q�f�ɂ���
			AND		EBX, 0x00ff0000								// src:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			AND		EAX, 0xff0000ff								// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����
			AND		ECX, 0x00ff0000								// dst:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16
			MOV		bgRateSrc, EAX

			SHL		ECX, 8
			MOV		EAX, EDX

			ROR		EAX, 8
			OR		EBX, ECX									// d�q�O�Os�q�ɂȂ���

			AND		EAX, 0xff0000ff
			MOV		rRate, EBX

			MOV		bgRateDst, EAX
			MOV		EDI, lpDst

			MOV		ESI, lpSrc
			MOV		EAX, EIY

			MOV		nEYCnt, EAX				// UnPair			// EY = InitializeY;

		LoopY_11:
			MOV		EAX, EIX				// UnPair
			MOV		lpSrcBack, ESI

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_11: // 34�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������58�N���b�N)
			MOV		EAX, [ESI]									// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			CMP		EAX, colKey
			JE		SkipColKey_11

			MOV		ECX, [EDI]				// 2
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0xff000000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			SHR		EDX, 8					// 3
			MOV		SrcBackup, EAX			// 1

			MOV		ECX, EDX				// 3
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		EAX, EBX				// 2// UnPair

			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

		SkipColKey_11:
			ADD		EDI, 4										// Dst�����ɐi�߂�
			MOV		EAX, EX

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_11										// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_11:
			DEC		i
			JNZ		LoopX_11

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_11										// if ( EY >= 0 )

			ADD		ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			MOV		EAX, EY2

			SUB		nEYCnt, EAX									// Y�̕␳
		SkipY_11:
			DEC		j
			JNZ		LoopY_11
		}
	} // if

	
	return 0;
} // BlendBlt


//////////////////////////////////////
//	BlendBltFast
//	�����F������RGB�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::BlendBltFast(CDIB32* lpDIB32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
							   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BlendBltFast��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::BlendBltFast��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);			// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:��	 RGB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		i, j;


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EAX, dwSrcRGBRate
			MOV		ECX, dwDstRGBRate

			MOV		EBX, EAX
			MOV		EDX, ECX

			ROR		EAX, 8										// ���q�f�a���a���q�f�ɂ���
			AND		EBX, 0x00ff0000								// src:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			AND		EAX, 0xff0000ff								// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����
			AND		ECX, 0x00ff0000								// dst:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16
			MOV		bgRateSrc, EAX

			SHL		ECX, 8
			MOV		EAX, EDX

			ROR		EAX, 8
			OR		EBX, ECX									// d�q�O�Os�q�ɂȂ���

			AND		EAX, 0xff0000ff
			MOV		bgRateDst, EAX

			MOV		rRate, EBX				// UnPair

		LoopX_12: // 34�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������59�N���b�N)
			MOV		EAX, [ESI]
			ADD		ESI, 4

			MOV		ECX, [EDI]				// 2
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0xff000000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			SHR		EDX, 8					// 3
			MOV		SrcBackup, EAX			// 1

			MOV		ECX, EDX				// 3
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		EAX, EBX				// 2// UnPair

			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_12				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_12
		}
	}// if
// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:�L	 RGB�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;


		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EAX, dwSrcRGBRate
			MOV		ECX, dwDstRGBRate

			MOV		EBX, EAX
			MOV		EDX, ECX

			ROR		EAX, 8										// ���q�f�a���a���q�f�ɂ���
			AND		EBX, 0x00ff0000								// src:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			AND		EAX, 0xff0000ff								// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����
			AND		ECX, 0x00ff0000								// dst:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16
			MOV		bgRateSrc, EAX

			SHL		ECX, 8
			MOV		EAX, EDX

			ROR		EAX, 8
			OR		EBX, ECX									// d�q�O�Os�q�ɂȂ���

			AND		EAX, 0xff0000ff
			MOV		rRate, EBX

			MOV		bgRateDst, EAX
			MOV		EDI, lpDst

			MOV		ESI, lpSrc
			MOV		EAX, EIY

			MOV		nEYCnt, EAX				// UnPair			// EY = InitializeY;

		LoopY_13:
			MOV		lpSrcBack, ESI
			MOV		EAX, EIX				// UnPair

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_13: // 37�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������61�N���b�N)
			MOV		EAX, [ESI]									// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			MOV		ECX, [EDI]				// 2
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0xff000000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			SHR		EDX, 8					// 3
			MOV		SrcBackup, EAX			// 1

			MOV		ECX, EDX				// 3
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		EAX, EBX				// 2// UnPair

			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// Dst�����ɐi�߂�
			MOV		EAX, EX

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_13										// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_13:
			DEC		i
			JNZ		LoopX_13

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_13									// if ( EY >= 0 )

			ADD		ESI, lPitchSrc				// 1���C�������Z���āA���̍s��
			MOV		EAX, EY2

			SUB		nEYCnt, EAX									// Y�̕␳

		SkipY_13:
			DEC		j
			JNZ		LoopY_13
		}
	} // if

	
	return 0;
} // BlendBltFast


//////////////////////////////////////
//	AddColorBlt
//	�����F�L���Add�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::AddColorBlt(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::AddColorBlt��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::AddColorBlt��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);			// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:��	 ADD�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

		LoopX_14: // 15�N���b�N��T�C�N��
			MOV		EAX, [ESI]
			ADD		ESI, 4

			CMP		EAX, colKey
			JE		SkipColKey_14

			MOV		EBX, [EDI]
			MOV		ECX, EAX

			AND		EAX, EBX									// (src&dst)
			XOR		ECX, EBX									// (src^dst)

			SHL		EAX, 1										// <<1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, [ESI-4]

			ADD		EAX, Mask7f									// + Mask7f
			MOV		ECX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - mask)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

		SkipColKey_14:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_14				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_14
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:�L	 RGB�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;


		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EDI, lpDst
			MOV		EAX, EIY

			MOV		ESI, lpSrc
			MOV		nEYCnt, EAX				// UnPair			// EY = InitializeY;

		LoopY_15:
			MOV		lpSrcBack, ESI
			MOV		EAX, EIX				// UnPair

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_15: // 19(8)�N���b�N��T�C�N��
			MOV		EAX, [ESI]				// UnPair			// �s�N�Z���̃R�s�[

			CMP		EAX, colKey
			JE		SkipColKey_15

			MOV		EBX, [EDI]				// UnPair
			MOV		ECX, EAX

			AND		EAX, EBX									// (src&dst)
			XOR		ECX, EBX									// (src^dst)

			SHL		EAX, 1										// <<1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, [ESI]

			ADD		EAX, Mask7f									// + Mask7f
			MOV		ECX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - mask)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

		SkipColKey_15:
			ADD		ESI, AddSrcPixel							// �������̉��Z
			ADD		EDI, 4										// Dst�����ɐi�߂�

			MOV		EAX, EX					// UnPair

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_15										// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_15:
			DEC		i
			JNZ		LoopX_15

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_15									// if ( EY >= 0 )

			ADD		ESI, lPitchSrc							// 1���C�������Z���āA���̍s��
			MOV		EAX, EY2

			SUB		nEYCnt, EAX									// Y�̕␳

		SkipY_15:
			DEC		j
			JNZ		LoopY_15
		}
	} // if

	
	return 0;
} // AddColorBlt


//////////////////////////////////////
//	AddColorBltFast
//	�����F������Add�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::AddColorBltFast(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::AddColorBltFast��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::AddColorBltFast��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);			// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:��	 ADD�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

		LoopX_16: // 14�N���b�N��T�C�N��
			MOV		EAX, [ESI]
			ADD		ESI, 4

			MOV		EBX, [EDI]
			MOV		ECX, EAX

			AND		EAX, EBX									// (src&dst)
			XOR		ECX, EBX									// (src^dst)

			SHL		EAX, 1										// <<1
			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, [ESI-4]

			ADD		EAX, Mask7f									// + Mask7f
			MOV		ECX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - mask)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_16				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_16
		}
	}// if
// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:�L	 ADD�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;


		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EDI, lpDst
			MOV		EAX, EIY

			MOV		ESI, lpSrc
			MOV		nEYCnt, EAX				// UnPair			// EY = InitializeY;

		LoopY_17:
			MOV		lpSrcBack, ESI
			MOV		EAX, EIX				// UnPair

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_17: // 18�N���b�N��T�C�N��
			MOV		EAX, [ESI]				// UnPair			// �s�N�Z���̃R�s�[

			MOV		EBX, [EDI]				// UnPair
			MOV		ECX, EAX

			AND		EAX, EBX									// (src&dst)
			XOR		ECX, EBX									// (src^dst)

			SHL		EAX, 1										// <<1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, [ESI]

			ADD		EAX, Mask7f									// + Mask7f
			MOV		ECX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - mask)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		ESI, AddSrcPixel							// �������̉��Z
			ADD		EDI, 4										// Dst�����ɐi�߂�

			MOV		EAX, EX					// UnPair

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_17										// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_17:
			DEC		i
			JNZ		LoopX_17

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_17										// if ( EY >= 0 )

			ADD		ESI, lPitchSrc
			MOV		EAX, EY2

			SUB		nEYCnt, EAX									// Y�̕␳
		SkipY_17:
			DEC		j
			JNZ		LoopY_17
		}
	} // if

	
	return 0;
} // AddColorBltFast


//////////////////////////////////////
//	SubColorBlt
//	�����F�L���Sub�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::SubColorBlt(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::SubColorBlt��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::SubColorBlt��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);			// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:��	 SUB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

		LoopX_18: // 16�N���b�N��T�C�N��
			MOV		EAX, [ESI]
			ADD		ESI, 4

			CMP		EAX, colKey
			JE		SkipColKey_18

			MOV		ECX, EAX
			MOV		EBX, [EDI]

			NOT		EBX						// UnPair

			AND		EAX, EBX									// (src & ~dst)
			XOR		ECX, EBX									// (src ^ ~dst)

			SHL		EAX, 1										// << 1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8					// UnPair			//	(c >> 8)

			ADD		EAX, Mask7f									// + Mask7f
			MOV		EBX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			MOV		ECX, [ESI-4]

			OR		EBX, EAX									// dst | c
			OR		ECX, EAX									// src | c

			SUB		EBX, ECX				// UnPair			// () - ()

			MOV		[EDI], EBX				// UnPair

		SkipColKey_18:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_18				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_18
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:�L	 SUB�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;


		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EDI, lpDst
			MOV		EAX, EIY

			MOV		ESI, lpSrc
			MOV		nEYCnt, EAX				// UnPair			// EY = InitializeY;

		LoopY_19:
			MOV		lpSrcBack, ESI
			MOV		EAX, EIX				// UnPair

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_19: // 20(8)�N���b�N��T�C�N��
			MOV		EAX, [ESI]				// UnPair			// �s�N�Z���̃R�s�[

			CMP		EAX, colKey
			JE		SkipColKey_19

			MOV		ECX, EAX
			MOV		EBX, [EDI]

			NOT		EBX						// UnPair

			AND		EAX, EBX									// (src & ~dst)
			XOR		ECX, EBX									// (src ^ ~dst)

			SHL		EAX, 1										// << 1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8					// UnPair			//	(c >> 8)

			ADD		EAX, Mask7f									// + Mask7f
			MOV		EBX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			MOV		ECX, [ESI]

			OR		EBX, EAX									// dst | c
			OR		ECX, EAX									// src | c

			SUB		EBX, ECX				// UnPair			// () - ()

			MOV		[EDI], EBX				// UnPair

		SkipColKey_19:
			ADD		ESI, AddSrcPixel							// �������̉��Z
			ADD		EDI, 4										// Dst�����ɐi�߂�

			MOV		EAX, EX					// UnPair

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_19										// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_19:
			DEC		i
			JNZ		LoopX_19

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_19									// if ( EY >= 0 )

			ADD		ESI, lPitchSrc							// 1���C�������Z���āA���̍s��
			MOV		EAX, EY2

			SUB		nEYCnt, EAX									// Y�̕␳
	
		SkipY_19:
			DEC		j
			JNZ		LoopY_19
		}
	} // if

	
	return 0;
} // SubColorBlt


//////////////////////////////////////
//	SubColorBltFast
//	�����F������Sub�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::SubColorBltFast(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::SubColorBltFast��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::SubColorBltFast��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);										// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:��	 SUB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

		LoopX_20: // 15�N���b�N��T�C�N��
			MOV		EAX, [ESI]
			ADD		ESI, 4

			MOV		ECX, EAX
			MOV		EBX, [EDI]

			NOT		EBX						// UnPair

			AND		EAX, EBX									// (src & ~dst)
			XOR		ECX, EBX									// (src ^ ~dst)

			SHL		EAX, 1										// << 1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8					// UnPair			//	(c >> 8)

			ADD		EAX, Mask7f									// + Mask7f
			MOV		EBX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			MOV		ECX, [ESI-4]

			OR		EBX, EAX									// dst | c
			OR		ECX, EAX									// src | c

			SUB		EBX, ECX				// UnPair			// () - ()

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_20				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_20
		}
	}// if
// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:�L	 SUB�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;


		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EDI, lpDst
			MOV		EAX, EIY

			MOV		ESI, lpSrc
			MOV		nEYCnt, EAX				// UnPair			// EY = InitializeY;

		LoopY_21:
			MOV		lpSrcBack, ESI
			MOV		EAX, EIX				// UnPair

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_21: // 19�N���b�N��T�C�N��
			MOV		EAX, [ESI]									// �s�N�Z���̃R�s�[

			MOV		ECX, EAX
			MOV		EBX, [EDI]

			NOT		EBX						// UnPair

			AND		EAX, EBX									// (src & ~dst)
			XOR		ECX, EBX									// (src ^ ~dst)

			SHL		EAX, 1										// << 1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8					// UnPair			//	(c >> 8)

			ADD		EAX, Mask7f									// + Mask7f
			MOV		EBX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			MOV		ECX, [ESI]

			OR		EBX, EAX									// dst | c
			OR		ECX, EAX									// src | c

			SUB		EBX, ECX				// UnPair			// () - ()

			MOV		[EDI], EBX				// UnPair

			ADD		ESI, AddSrcPixel							// �������̉��Z
			ADD		EDI, 4										// Dst�����ɐi�߂�

			MOV		EAX, EX					// UnPair

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_21									// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_21:
			DEC		i
			JNZ		LoopX_21

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_21										// if ( EY >= 0 )

			ADD		ESI, lPitchSrc							// 1���C�������Z���āA���̍s��
			MOV		EAX, EY2

			SUB		nEYCnt, EAX									// Y�̕␳

		SkipY_21:
			DEC		j
			JNZ		LoopY_21
		}
	} // if


	return 0;
} // SubColorBltFast


////////////////////////////////////////////////////////////////////
//	�~���[�L���`�]��
////////////////////////////////////////////////////////////////////
//////////////////////////////////////
//	BltM
//	�����F�L��̓]��
//////////////////////////////////////
LRESULT CDIB32P5::BltM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BltM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::BltM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		ECX, nWidth
			MOV		EDX, nHeight

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EBX, colKey				// UnPair

		LoopY_1:

		LoopX_1:	// 5(4)�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]
			ADD		ESI, -4

			CMP		EAX, EBX
			JE		Skip_1

			MOV		[EDI], EAX				// UnPair

		Skip_1:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		ECX

			JNZ		LoopX_1					// UnPair

			MOV		ECX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			DEC		EDX

			JNZ		LoopY_1					// UnPair
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		int		i, j;
		BYTE*	lpSrcBack;
		DWORD	lPitchSrc =	 p->m_lPitch;

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;

		LoopY_2:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX				// UnPair			// EY = InitializeX;

		LoopX_2: // 7(6)�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]								// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			CMP		EAX, colKey
			JE		SkipColKey_2

			MOV		[EDI], EAX				// UnPair

		SkipColKey_2:
			ADD		EDI, 4					// UnPair			// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_2										// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_2:
			DEC		i
			JNZ		LoopX_2

			MOV		ESI, lpSrcBack;								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_2					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc
			SUB		EDX, EY2									// Y�̕␳

		SkipY_2:
			DEC		j
			JNZ		LoopY_2
		}
	} // if


	return 0;
} // BltM


//////////////////////////////////////
//	BltFastM
//	�����F�����̓]��
//////////////////////////////////////
LRESULT CDIB32P5::BltFastM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BltFastM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::BltFastM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, nHeight
			MOV		ECX, nWidth

		LoopX_3: // 3�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]
			ADD		ESI, -4

			MOV		[EDI], EAX 
			ADD		EDI, 4

			DEC		ECX
			JNZ		LoopX_3

			MOV		ECX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			DEC		EDX

			JNZ		LoopX_3					// UnPair
		}
	} // if
// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:�L	 --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;

		LoopY_4:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX				// UnPair			// EY = InitializeX;

		LoopX_4: // 5�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]								// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			MOV		[EDI], EAX
			ADD		EDI, 4										// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_4										// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_4:
			DEC		i
			JNZ		LoopX_4

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_4					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc
			SUB		EDX, EY2									// Y�̕␳

		SkipY_4:
			DEC		j
			JNZ		LoopY_4
		}
	} // if

	
	return 0;
} // BltFastM


//////////////////////////////////////
//	BlendBltHalfM
//	�����F�L��̂P�F�P�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::BlendBltHalfM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BlendBltHalfM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::BlendBltHalfM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:��	 1:1�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	colKey = p->m_dwColorKey;
		int		i, j;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		EDI, lpDst
			MOV		ESI, lpSrc

			MOV		i, EAX
			MOV		j, EBX

		LoopY_5:

		LoopX_5:	// 11(4)�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]
			ADD		ESI, -4

			CMP		EAX, colKey
			JE		Skip_5

			MOV		EBX, [EDI]
			MOV		ECX, EAX

			MOV		EDX, EBX
			AND		EAX, EBX

			XOR		ECX, EDX				// UnPair

			SHR		ECX, 1
			MOV		EBX, Mask7f

			AND		ECX, EBX				// UnPair

			ADD		EAX, ECX				// UnPair

			MOV		[EDI], EAX				// UnPair

		Skip_5:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_5					// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			MOV		i, EAX

			DEC		j
			JNZ		LoopY_5
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 1:1�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	colKey = p->m_dwColorKey;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;

		LoopY_6:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX				// UnPair			// EY = InitializeX;

		LoopX_6: // 12(6)�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]			// UnPair			// �s�N�Z���̃R�s�[

			CMP		EAX, colKey
			JE		SkipColKey_6

			MOV		EBX, [EDI]				// UnPair

			XOR		EAX, EBX				// UnPair

			SHR		EAX, 1					// UnPair

			AND		EAX, Mask7f
			AND		EBX, [ESI-4]

			ADD		EAX, EBX				// UnPair

			MOV		[EDI], EAX				// UnPair

		SkipColKey_6:
			ADD		ESI, AddSrcPixel							// �������̉��Z
			ADD		EDI, 4										// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_6										// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_6:
			DEC		i
			JNZ		LoopX_6

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_6					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc
			SUB		EDX, EY2									// Y�̕␳

		SkipY_6:
			DEC		j
			JNZ		LoopY_6
		}
	} // if

	
	return 0;
} // BlendBltHalfM


//////////////////////////////////////
//	BlendBltFastHalfM
//	�����F�����̂P�F�P�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::BlendBltFastHalfM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BlendBltFastHalfM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::BlendBltFastHalfM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:��	 1:1�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		EDI, lpDst
			MOV		ESI, lpSrc

			MOV		i, EAX
			MOV		j, EBX

		LoopX_7:	// 8�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]			// UnPair
			ADD		ESI, -4

			MOV		ECX, EAX
			MOV		EBX, [EDI]

			AND		EAX, EBX
			XOR		ECX, EBX

			SHR		ECX, 1
			MOV		EBX, Mask7f

			AND		ECX, EBX				// UnPair

			ADD		EAX, ECX				// UnPair

			MOV		[EDI], EAX
			ADD		EDI, 4										// EDI���ɐi�߂�

			DEC		i
			JNZ		LoopX_7

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_7
		}
	}// if
// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:�L	 1:1�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;

		LoopY_8:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX				// UnPair			// EY = InitializeX;

		LoopX_8: // 10�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]			// UnPair			// �s�N�Z���̃R�s�[

			MOV		EBX, [EDI]				// UnPair

			XOR		EAX, EBX				// UnPair

			SHR		EAX, 1					// UnPair

			AND		EAX, Mask7f
			AND		EBX, [ESI-4]

			ADD		EAX, EBX				// UnPair

			MOV		[EDI], EAX				// UnPair

			ADD		ESI, AddSrcPixel							// �������̉��Z
			ADD		EDI, 4										// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_8										// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_8:
			DEC		i
			JNZ		LoopX_8

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_8					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc
			SUB		EDX, EY2									// Y�̕␳

		SkipY_8:
			DEC		j
			JNZ		LoopY_8
		}
	} // if


	return 0;
} // BlendBltHalfM


//////////////////////////////////////
//	BlendBltM
//	�����F�L���RGB�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::BlendBltM(CDIB32* lpDIB32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
						   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BlendBltM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::BlendBltM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:��	 RGB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EAX, dwSrcRGBRate
			MOV		ECX, dwDstRGBRate

			MOV		EBX, EAX
			MOV		EDX, ECX

			ROR		EAX, 8										// ���q�f�a���a���q�f�ɂ���
			AND		EBX, 0x00ff0000								// src:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			AND		EAX, 0xff0000ff								// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����
			AND		ECX, 0x00ff0000								// dst:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16
			MOV		bgRateSrc, EAX

			SHL		ECX, 8
			MOV		EAX, EDX

			ROR		EAX, 8
			OR		EBX, ECX									// d�q�O�Os�q�ɂȂ���

			AND		EAX, 0xff0000ff
			MOV		bgRateDst, EAX

			MOV		rRate, EBX				// UnPair

		LoopX_9: // 35�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������59�N���b�N)
			MOV		EAX, [ESI-4]
			ADD		ESI, -4

			CMP		EAX, colKey
			JE		SkipColKey_9

			MOV		ECX, [EDI]				// 2
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0xff000000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			SHR		EDX, 8					// 3
			MOV		SrcBackup, EAX			// 1

			MOV		ECX, EDX				// 3
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		EAX, EBX				// 2// UnPair

//			MUL		bgRateSrc				// UnPair
			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

		SkipColKey_9:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_9				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_9
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 RGB�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EAX, dwSrcRGBRate
			MOV		ECX, dwDstRGBRate

			MOV		EBX, EAX
			MOV		EDX, ECX

			ROR		EAX, 8										// ���q�f�a���a���q�f�ɂ���
			AND		EBX, 0x00ff0000								// src:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			AND		EAX, 0xff0000ff								// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����
			AND		ECX, 0x00ff0000								// dst:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16
			MOV		bgRateSrc, EAX

			SHL		ECX, 8
			MOV		EAX, EDX

			ROR		EAX, 8
			OR		EBX, ECX									// d�q�O�Os�q�ɂȂ���

			AND		EAX, 0xff0000ff
			MOV		rRate, EBX

			MOV		bgRateDst, EAX
			MOV		EDI, lpDst

			MOV		ESI, lpSrc
			MOV		EAX, EIY

			MOV		nEYCnt, EAX				// UnPair			// EY = InitializeY;

		LoopY_10:
			MOV		lpSrcBack, ESI
			MOV		EAX, EIX				// UnPair			// �y�A�ɂȂ�Ȃ����ǁE�E�E�܂��Ay���[�v�������炢����:p

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_10: // 34�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������58�N���b�N)
			MOV		EAX, [ESI-4]								// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			CMP		EAX, colKey
			JE		SkipColKey_10

			MOV		ECX, [EDI]				// 2
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0xff000000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			SHR		EDX, 8					// 3
			MOV		SrcBackup, EAX			// 1

			MOV		ECX, EDX				// 3
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		EAX, EBX				// 2// UnPair

//			MUL		bgRateSrc				// UnPair
			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

		SkipColKey_10:
			ADD		EDI, 4										// Dst�����ɐi�߂�
			MOV		EAX, EX

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_10									// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_10:
			DEC		i
			JNZ		LoopX_10

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY										// Y�̑��������Z

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_10									// if ( EY >= 0 )

			MOV		EAX, EY2									// Y�̕␳
			ADD		ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

			SUB		nEYCnt, EAX				// UnPair			// Y�̕␳

		SkipY_10:
			DEC		j
			JNZ		LoopY_10
		}
	} // if

	
	return 0;
} // BlendBltM


//////////////////////////////////////
//	BlendBltFastM
//	�����F������RGB�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::BlendBltFastM(CDIB32* lpDIB32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
							   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BlendBltFastM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::BlendBltFastM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:��	 RGB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		i, j;

				nSrcWidth = nSrcWidth +	 p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EAX, dwSrcRGBRate
			MOV		ECX, dwDstRGBRate

			MOV		EBX, EAX
			MOV		EDX, ECX

			ROR		EAX, 8										// ���q�f�a���a���q�f�ɂ���
			AND		EBX, 0x00ff0000								// src:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			AND		EAX, 0xff0000ff								// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����
			AND		ECX, 0x00ff0000								// dst:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16
			MOV		bgRateSrc, EAX

			SHL		ECX, 8
			MOV		EAX, EDX

			ROR		EAX, 8
			OR		EBX, ECX									// d�q�O�Os�q�ɂȂ���

			AND		EAX, 0xff0000ff
			MOV		bgRateDst, EAX

			MOV		rRate, EBX				// UnPair

		LoopX_11: // 34�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������59�N���b�N)
			MOV		EAX, [ESI-4]
			ADD		ESI, -4

			MOV		ECX, [EDI]				// 2
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0xff000000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			SHR		EDX, 8					// 3
			MOV		SrcBackup, EAX			// 1

			MOV		ECX, EDX				// 3
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		EAX, EBX				// 2// UnPair

//			MUL		bgRateSrc				// UnPair
			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_11				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_11
		}
	}// if
// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:�L	 RGB�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EAX, dwSrcRGBRate
			MOV		ECX, dwDstRGBRate

			MOV		EBX, EAX
			MOV		EDX, ECX

			ROR		EAX, 8										// ���q�f�a���a���q�f�ɂ���
			AND		EBX, 0x00ff0000								// src:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			AND		EAX, 0xff0000ff								// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����
			AND		ECX, 0x00ff0000								// dst:�O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16
			MOV		bgRateSrc, EAX

			SHL		ECX, 8
			MOV		EAX, EDX

			ROR		EAX, 8
			OR		EBX, ECX									// d�q�O�Os�q�ɂȂ���

			AND		EAX, 0xff0000ff
			MOV		rRate, EBX

			MOV		bgRateDst, EAX
			MOV		EDI, lpDst

			MOV		ESI, lpSrc
			MOV		EAX, EIY

			MOV		nEYCnt, EAX				// UnPair			// EY = InitializeY;

		LoopY_12:
			MOV		lpSrcBack, ESI
			MOV		EAX, EIX				// UnPair			// �y�A�ɂȂ�Ȃ����ǁE�E�E�܂��Ay���[�v�������炢����:p

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_12: // 37�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������61�N���b�N)
			MOV		EAX, [ESI-4]								// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			MOV		ECX, [EDI]				// 2
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0xff000000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			SHR		EDX, 8					// 3
			MOV		SrcBackup, EAX			// 1

			MOV		ECX, EDX				// 3
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		EAX, EBX				// 2// UnPair

//			MUL		bgRateSrc				// UnPair
			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// Dst�����ɐi�߂�
			MOV		EAX, EX

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_12									// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_12:
			DEC		i
			JNZ		LoopX_12

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY										// Y�̑��������Z

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_12										// if ( EY >= 0 )

			MOV		EAX, EY2									// Y�̕␳
			ADD		ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

			SUB		nEYCnt, EAX				// UnPair			// Y�̕␳

		SkipY_12:
			DEC		j
			JNZ		LoopY_12
		}
	} // if

	
	return 0;
} // BlendBltFastM


//////////////////////////////////////
//	AddColorBltM
//	�����F�L���Add�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::AddColorBltM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::AddColorBltM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::AddColorBltM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:��	 ADD�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

		LoopX_13: // 15�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]
			ADD		ESI, -4

			CMP		EAX, colKey
			JE		SkipColKey_13

			MOV		EBX, [EDI]
			MOV		ECX, EAX

			AND		EAX, EBX									// (src&dst)
			XOR		ECX, EBX									// (src^dst)

			SHL		EAX, 1										// <<1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, [ESI]

			ADD		EAX, Mask7f									// + Mask7f
			MOV		ECX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - mask)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

		SkipColKey_13:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_13				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_13
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 RGB�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EDI, lpDst
			MOV		EAX, EIY

			MOV		ESI, lpSrc
			MOV		nEYCnt, EAX				// UnPair			// EY = InitializeY;

		LoopY_14:
			MOV		EAX, EIX				// UnPair			// �y�A�ɂȂ�Ȃ����ǁE�E�E�܂��Ay���[�v�������炢����:p
			MOV		lpSrcBack, ESI

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_14: // 19(8)�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]			// UnPair			// �s�N�Z���̃R�s�[

			CMP		EAX, colKey
			JE		SkipColKey_14

			MOV		EBX, [EDI]				// UnPair
			MOV		ECX, EAX

			AND		EAX, EBX									// (src&dst)
			XOR		ECX, EBX									// (src^dst)

			SHL		EAX, 1										// <<1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, [ESI-4]

			ADD		EAX, Mask7f									// + Mask7f
			MOV		ECX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - mask)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

		SkipColKey_14:
			ADD		ESI, AddSrcPixel							// �������̉��Z
			ADD		EDI, 4										// Dst�����ɐi�߂�

			MOV		EAX, EX					// UnPair

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_14									// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_14:
			DEC		i
			JNZ		LoopX_14

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY										// Y�̑��������Z

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_14										// if ( EY >= 0 )

			MOV		EAX, EY2									// Y�̕␳
			ADD		ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

			SUB		nEYCnt, EAX				// UnPair			// Y�̕␳

		SkipY_14:
			DEC		j
			JNZ		LoopY_14
		}
	} // if

	
	return 0;
} // AddColorBltM


//////////////////////////////////////
//	AddColorBltFastM
//	�����F������Add�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::AddColorBltFastM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::AddColorBltFastM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::AddColorBltFastM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:��	 ADD�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

		LoopX_15: // 14�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]
			ADD		ESI, -4

			MOV		EBX, [EDI]
			MOV		ECX, EAX

			AND		EAX, EBX									// (src&dst)
			XOR		ECX, EBX									// (src^dst)

			SHL		EAX, 1										// <<1
			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, [ESI]

			ADD		EAX, Mask7f									// + Mask7f
			MOV		ECX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - mask)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_15				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_15
		}
	}// if
// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:�L	 ADD�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EDI, lpDst
			MOV		EAX, EIY

			MOV		ESI, lpSrc
			MOV		nEYCnt, EAX				// UnPair			// EY = InitializeY;

		LoopY_16:
			MOV		EAX, EIX				// UnPair			// �y�A�ɂȂ�Ȃ����ǁE�E�E�܂��Ay���[�v�������炢����:p
			MOV		lpSrcBack, ESI

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_16: // 18�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]			// UnPair			// �s�N�Z���̃R�s�[

			MOV		EBX, [EDI]
			MOV		ECX, EAX

			AND		EAX, EBX									// (src&dst)
			XOR		ECX, EBX									// (src^dst)

			SHL		EAX, 1										// <<1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, [ESI-4]

			ADD		EAX, Mask7f									// + Mask7f
			MOV		ECX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - mask)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		ESI, AddSrcPixel							// �������̉��Z
			ADD		EDI, 4										// Dst�����ɐi�߂�

			MOV		EAX, EX					// UnPair

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_16									// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_16:
			DEC		i
			JNZ		LoopX_16

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY										// Y�̑��������Z

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_16										// if ( EY >= 0 )

			MOV		EAX, EY2									// Y�̕␳
			ADD		ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

			SUB		nEYCnt, EAX				// UnPair			// Y�̕␳

		SkipY_16:
			DEC		j
			JNZ		LoopY_16
		}
	} // if

	
	return 0;
} // AddColorBltFastM


//////////////////////////////////////
//	SubColorBltM
//	�����F�L���Sub�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::SubColorBltM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::SubColorBltM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::SubColorBltM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:��	 SUB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

		LoopX_17: // 16�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]
			ADD		ESI, -4

			CMP		EAX, colKey
			JE		SkipColKey_17

			MOV		ECX, EAX
			MOV		EBX, [EDI]

			NOT		EBX						// UnPair

			AND		EAX, EBX									// (src & ~dst)
			XOR		ECX, EBX									// (src ^ ~dst)

			SHL		EAX, 1										// << 1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8					// UnPair			//	(c >> 8)

			ADD		EAX, Mask7f									// + Mask7f
			MOV		EBX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			MOV		ECX, [ESI]

			OR		EBX, EAX									// dst | c
			OR		ECX, EAX									// src | c

			SUB		EBX, ECX				// UnPair			// () - ()

			MOV		[EDI], EBX				// UnPair

		SkipColKey_17:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_17				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_17
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 SUB�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EDI, lpDst
			MOV		EAX, EIY

			MOV		ESI, lpSrc
			MOV		nEYCnt, EAX				// UnPair			// EY = InitializeY;

		LoopY_18:
			MOV		EAX, EIX				// UnPair			// �y�A�ɂȂ�Ȃ����ǁE�E�E�܂��Ay���[�v�������炢����:p
			MOV		lpSrcBack, ESI

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_18: // 20(8)�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]			// UnPair			// �s�N�Z���̃R�s�[

			CMP		EAX, colKey
			JE		SkipColKey_18

			MOV		ECX, EAX
			MOV		EBX, [EDI]

			NOT		EBX						// UnPair

			AND		EAX, EBX									// (src & ~dst)
			XOR		ECX, EBX									// (src ^ ~dst)

			SHL		EAX, 1										// << 1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8					// UnPair			//	(c >> 8)

			ADD		EAX, Mask7f									// + Mask7f
			MOV		EBX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			MOV		ECX, [ESI-4]

			OR		EBX, EAX									// dst | c
			OR		ECX, EAX									// src | c

			SUB		EBX, ECX				// UnPair			// () - ()

			MOV		[EDI], EBX				// UnPair

		SkipColKey_18:
			ADD		ESI, AddSrcPixel							// �������̉��Z
			ADD		EDI, 4										// Dst�����ɐi�߂�

			MOV		EAX, EX					// UnPair

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_18									// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_18:
			DEC		i
			JNZ		LoopX_18

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY										// Y�̑��������Z

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_18										// if ( EY >= 0 )

			MOV		EAX, EY2									// Y�̕␳
			ADD		ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

			SUB		nEYCnt, EAX				// UnPair			// Y�̕␳

		SkipY_18:
			DEC		j
			JNZ		LoopY_18
		}
	} // if

	
	return 0;
} // SubColorBltM


//////////////////////////////////////
//	SubColorBltFastM
//	�����F������Sub�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::SubColorBltFastM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::SubColorBltFastM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::SubColorBltFastM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:��	 SUB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

		LoopX_19: // 15�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]
			ADD		ESI, -4

			MOV		ECX, EAX
			MOV		EBX, [EDI]

			NOT		EBX						// UnPair

			AND		EAX, EBX									// (src & ~dst)
			XOR		ECX, EBX									// (src ^ ~dst)

			SHL		EAX, 1										// << 1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8					// UnPair			//	(c >> 8)

			ADD		EAX, Mask7f									// + Mask7f
			MOV		EBX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			MOV		ECX, [ESI]

			OR		EBX, EAX									// dst | c
			OR		ECX, EAX									// src | c

			SUB		EBX, ECX				// UnPair			// () - ()

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_19				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_19
		}
	}// if
// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:�L	 SUB�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EDI, lpDst
			MOV		EAX, EIY

			MOV		ESI, lpSrc
			MOV		nEYCnt, EAX									// EY = InitializeY;

		LoopY_20:
			MOV		EAX, EIX				// UnPair			// �y�A�ɂȂ�Ȃ����ǁE�E�E�܂��Ay���[�v�������炢����:p
			MOV		lpSrcBack, ESI

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_20: // 19�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]								// �s�N�Z���̃R�s�[

			MOV		ECX, EAX
			MOV		EBX, [EDI]

			NOT		EBX						// UnPair

			AND		EAX, EBX									// (src & ~dst)
			XOR		ECX, EBX									// (src ^ ~dst)

			SHL		EAX, 1										// << 1
			AND		ECX, feMask									// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8					// UnPair			//	(c >> 8)

			ADD		EAX, Mask7f									// + Mask7f
			MOV		EBX, [EDI]

			XOR		EAX, Mask7f									// ^ Mask7f
			MOV		ECX, [ESI-4]

			OR		EBX, EAX									// dst | c
			OR		ECX, EAX									// src | c

			SUB		EBX, ECX				// UnPair			// () - ()

			MOV		[EDI], EBX				// UnPair

			ADD		ESI, AddSrcPixel							// �������̉��Z
			ADD		EDI, 4										// Dst�����ɐi�߂�

			MOV		EAX, EX					// UnPair

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_20									// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_20:
			DEC		i
			JNZ		LoopX_20

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY										// Y�̑��������Z

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_20										// if ( EY >= 0 )

			MOV		EAX, EY2									// Y�̕␳
			ADD		ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

			SUB		nEYCnt, EAX				// UnPair			// Y�̕␳

		SkipY_20:
			DEC		j
			JNZ		LoopY_20
		}
	} // if

	
	return 0;
} // SubColorBltFastM


////////////////////////////////////////////////////////////////////
//	���̃v���[���ɑ΂���G�t�F�N�g
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////
//	AddColorFast
//	�����F������ADD�u�����h�]�� (���v���[��)
//////////////////////////////////////////////////
LRESULT CDIB32P5::AddColorFast(DWORD dwAddRGB,LPRECT lpSrcRect)
{
// �N���b�s���O���̏���
	//	�\�[�X��`��NULL�Ȃ�΁A�S��
	if (lpSrcRect==NULL)
	{
		m_rcSrcRect = m_rcRect;
	}
	else
	{
		m_rcSrcRect = *lpSrcRect;
		if ( m_rcSrcRect.left > m_rcSrcRect.right )
		{
			return 1;	// invalid rect
		}
	}
	//	�N���b�v�̈�
	LPRECT lpClip;
	lpClip = &m_rcRect;

		// �N���b�s���O����
		// this yaneurao clipping algorithm is changed by tia
		int t;
		t = lpClip->left  - m_rcSrcRect.left;
		if ( t > 0 ) { m_rcSrcRect.left = lpClip->left; }
		t = lpClip->top	  - m_rcSrcRect.top;
		if ( t > 0 ) { m_rcSrcRect.top = lpClip->top; }
		t = m_rcSrcRect.right - lpClip->right;
		if ( t > 0 ) { m_rcSrcRect.right = lpClip->right; }
		t = m_rcSrcRect.bottom - lpClip->bottom;
		if ( t > 0 ) { m_rcSrcRect.bottom = lpClip->bottom; }

		//	invalid rect ?
		if (m_rcSrcRect.left >= m_rcSrcRect.right ||
			m_rcSrcRect.top	 >= m_rcSrcRect.bottom) return 1;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcSrcRect.right - m_rcSrcRect.left;
	int		nHeight = m_rcSrcRect.bottom - m_rcSrcRect.top;
	DWORD	nAddSrcWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
	DWORD*	lpSrc = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcSrcRect.left<<2) + m_rcSrcRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:��	 ADD�u�����h:�L	  --------------
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, dwAddRGB

			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

		LoopX_16: // 14�N���b�N��T�C�N��
			MOV		EAX, [ESI]
			ADD		ESI, 4

			MOV		EBX, EDI
			MOV		ECX, EAX

			MOV		EDX, EAX
			AND		EAX, EBX									// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8										//	(c >> 8)
			ADD		EAX, Mask7f									// + Mask7f

			ADD		EDX, EBX									// src + dst

			XOR		EAX, Mask7f				// UnPair			// ^ Mask7f

			SUB		EDX, EAX				// UnPair			// ( - mask)

			OR		EDX, EAX				// UnPair			// () | mask

			MOV		[ESI-4], EDX			// UnPair

			DEC		i
			JNZ		LoopX_16

			MOV		EAX, nWidth
			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�

			MOV		i, EAX
			DEC		j

			JNZ		LoopX_16				// UnPair
		}

	
	return 0;
} // AddColorFast

//////////////////////////////////////////////////
//	SubColorFast
//	�����F������Sub�u�����h�]�� (���v���[��)
//////////////////////////////////////////////////
LRESULT CDIB32P5::SubColorFast(DWORD dwSubRGB,LPRECT lpSrcRect)
{
// �N���b�s���O���̏���
	//	�\�[�X��`��NULL�Ȃ�΁A�S��
	if (lpSrcRect==NULL)
	{
		m_rcSrcRect = m_rcRect;
	}
	else
	{
		m_rcSrcRect = *lpSrcRect;
		if ( m_rcSrcRect.left > m_rcSrcRect.right )
		{
			return 1;	// invalid rect
		}
	}
	//	�N���b�v�̈�
	LPRECT lpClip;
	lpClip = &m_rcRect;

		// �N���b�s���O����
		// this yaneurao clipping algorithm is changed by tia
		int t;
		t = lpClip->left  - m_rcSrcRect.left;
		if ( t > 0 ) { m_rcSrcRect.left = lpClip->left; }
		t = lpClip->top	  - m_rcSrcRect.top;
		if ( t > 0 ) { m_rcSrcRect.top = lpClip->top; }
		t = m_rcSrcRect.right - lpClip->right;
		if ( t > 0 ) { m_rcSrcRect.right = lpClip->right; }
		t = m_rcSrcRect.bottom - lpClip->bottom;
		if ( t > 0 ) { m_rcSrcRect.bottom = lpClip->bottom; }

		//	invalid rect ?
		if (m_rcSrcRect.left >= m_rcSrcRect.right ||
			m_rcSrcRect.top	 >= m_rcSrcRect.bottom) return 1;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcSrcRect.right - m_rcSrcRect.left;
	int		nHeight = m_rcSrcRect.bottom - m_rcSrcRect.top;
	DWORD	nAddSrcWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
	DWORD*	lpSrc = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcSrcRect.left<<2) + m_rcSrcRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:��	 ADD�u�����h:�L	  --------------
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		int		i, j;


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, dwSubRGB

			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

		LoopX_20: // 15�N���b�N��T�C�N��
			MOV		EAX, [ESI]
			ADD		ESI, 4

			MOV		ECX, EAX
			MOV		EBX, EDI

			NOT		EBX						// UnPair

			MOV		EDX, EAX
			AND		EAX, EBX									// (src & ~dst)

			SHL		EAX, 1										// << 1
			XOR		ECX, EBX									// (src ^ ~dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask

			SHR		EAX, 8					// UnPair			//	(c >> 8)

			ADD		EAX, Mask7f				// UnPair			// + Mask7f

			XOR		EAX, Mask7f									// ^ Mask7f
			MOV		EBX, EDI

			OR		EBX, EAX									// dst | c
			OR		EDX, EAX									// src | c

			SUB		EBX, EDX				// UnPair			// () - ()

			MOV		[ESI-4], EBX			// UnPair

			DEC		i
			JNZ		LoopX_20

			MOV		EAX, nWidth
			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�

			MOV		i, EAX
			DEC		j

			JNZ		LoopX_20				// UnPair
		}

	
	return 0;
} // SubColorFast


////////////////////////////////////////////////////////////////////////////
//	PentiumMMX�p�̃��[�`��
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//	�~���[������`�]��
////////////////////////////////////////////////////////////////////
//////////////////////////////////////
//	Blt
//	�����F�L��̓]��
//////////////////////////////////////
LRESULT CDIB32PMMX::Blt(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::Blt��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::Blt��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;
	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);			// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			EDI, lpDst

			MOVQ		MM1, MM7
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM7, MM1			// UnPair			// ColKey

		LoopY_1:
			SHR			ECX, 1
			JNB			Skip_1									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM2, [EDI]
			MOVQ		MM1, MM0

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// src ^ ()
			ADD			EDI, 4

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_1

		Skip_1:
			SHR			ECX, 1
			JNB			LoopX_1									//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			MOVQ		MM2, [EDI]
			MOVQ		MM1, MM0

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// src ^ ()
			ADD			EDI, 8

			ADD			ESI, 8				// UnPair

			MOVQ		[EDI-8], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_1

		LoopX_1: // 12�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// 1// UnPair

			MOVQ		MM3, [ESI+8]		// 2
			MOVQ		MM1, MM0			// 1

			MOVQ		MM4, MM3			// 2
			PCMPEQD		MM1, MM7			// 1				// ColKey:0xffffffff NonColKey:0x00000000

			MOVQ		MM2, [EDI]			// 1
			PCMPEQD		MM4, MM7			// 2				// ColKey:0xffffffff NonColKey:0x00000000

			MOVQ		MM5, [EDI+8]		// 2
			PXOR		MM2, MM0			// 1				// (Src ^ Dst)

			PXOR		MM5, MM3			// 2				// (Src ^ Dst)
			PAND		MM2, MM1			// 1				// & mask

			PAND		MM5, MM4			// 2				// &mask
			PXOR		MM0, MM2			// 1				// Src ^ ()

			PXOR		MM3, MM5			// 2				// Src ^ ()
			ADD			ESI, 16

			MOVQ		[EDI], MM0			// UnPair

			MOVQ		[EDI+8], MM3		// UnPair

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_1				// UnPair

		EndLoop_1:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_1				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:�L	 --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		DWORD	lPitchSrc = p->m_lPitch;
		DWORD*	lpSrcBack;
	
		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			PXOR		MM1, MM1

			MOVQ		MM2, MM7
			PXOR		MM0, MM0

			PUNPCKLDQ	MM7, MM2								// ColKey
			MOV			ECX, nWidth

			MOV			EDX, nHeight
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_2:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX								// nExCnt = EIX;

		LoopX_2: // 10�N���b�N��T�C�N��
			MOVD		MM2, [ESI]			// UnPair			// *lpDst = *lpSrc;

			MOVD		MM4, [EDI]
			MOVQ		MM3, MM2

			PCMPEQD		MM3, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM4, MM2								// (Src ^ Dst)

			PAND		MM4, MM3								// & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM2, MM4								// Src ^ ()
			ADD			EDI, 4									// lpDst++;

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_2									// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_2:
			MOVD		[EDI-4], MM2		// UnPair

			DEC			ECX
			JNZ			LoopX_2

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_2									// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l

		SkipY_2:
			DEC			EDX
			JNZ			LoopY_2

			EMMS
		}
	} // if


	return 0;
} // Blt


//////////////////////////////////////
//	BltFast
//	�����F�����̓]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BltFast(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BltFast��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::BltFast��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);										// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

		LoopY_3:
			SHR			ECX, 1
			JNB			Skip_3									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI]			// UnPair

			ADD			ESI, 4				// AGI���

			MOVD		[EDI], MM0			// UnPair
			
			ADD			EDI, 4
			OR			ECX, ECX

			JZ			EndLoop_3			// UnPair

		Skip_3:
			SHR			ECX, 1
			JNB			LoopX_3									//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			ADD			ESI, 8				// AGI���

			MOVQ		[EDI], MM0			// UnPair

			ADD			EDI, 8
			OR			ECX, ECX

			JZ			EndLoop_3			// UnPair

		LoopX_3: // 6�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// UnPair

			MOVQ		MM3, [ESI+8]		// UnPair

			MOVQ		[EDI], MM0			// UnPair

			MOVQ		[EDI+8], MM3		// UnPair

			ADD			ESI, 16
			ADD			EDI, 16

			DEC			ECX
			JNZ			LoopX_3

		EndLoop_3:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_3				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:�L	 --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			PXOR		MM1, MM1
			PXOR		MM0, MM0

			MOV			ECX, nWidth
			MOV			EDX, nHeight

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;

		LoopY_4:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_4: // 7�N���b�N��T�C�N��
			MOVD		MM0, [ESI]			// UnPair			// *lpDst = *lpSrc;

			ADD			EDI, 4									// lpDst++;
			ADD			ESI, AddSrcPixel							// �������̉��Z

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_4									// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_4:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_4

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_4									// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l
		SkipY_4:
			DEC			EDX
			JNZ			LoopY_4

			EMMS
		}
	} // if

	
	return 0;
} // BltFast


//////////////////////////////////////
//	BlendBltHalf
//	�����F�L���1�F1�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BlendBltHalf(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BlendBltHalf��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::BlendBltHalf��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);										// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:��	 1:1�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	colKey = p->m_dwColorKey;


		_asm
		{
			MOVD		MM6, Mask7f
			MOV			EDX, nHeight

			MOVQ		MM1, MM6
			MOV			ECX, nWidth

			PUNPCKLDQ	MM6, MM1								// 0x007f7f7f
			MOVD		MM7, colKey								// �J���[�L�[��ݒ肷��

			MOV			EDI, lpDst
			MOVQ		MM1, MM7

			MOV			ESI, lpSrc
			PUNPCKLDQ	MM7, MM1								// ColKey

		LoopY_5:
			SHR			ECX, 1
			JNB			Skip_5									//	�������ۂ��̔���
			// Step1
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM1, [EDI]
			MOVQ		MM3, MM0

			PCMPEQD		MM3, MM7								//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0
			MOVQ		MM2, MM0

			PAND		MM0, MM1
			PXOR		MM2, MM1

			PSRLD		MM2, 1				// UnPair

			PAND		MM2, MM6			// UnPair

			PADDD		MM0, MM2			// UnPair

			PXOR		MM1, MM0			// UnPair			// (P ^ Dst)

			PAND		MM1, MM3			// UnPair			// () & mask

			PXOR		MM0, MM1								// P ^ ()
			ADD			EDI, 4

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_5

		Skip_5:
			SHR			ECX, 1
			JNB			LoopX_5								//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			MOVQ		MM1, [EDI]
			MOVQ		MM3, MM0

			PCMPEQD		MM3, MM7								//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0
			MOVQ		MM2, MM0

			PAND		MM0, MM1
			PXOR		MM2, MM1

			PSRLD		MM2, 1				// UnPair

			PAND		MM2, MM6			// UnPair

			PADDD		MM0, MM2			// UnPair

			PXOR		MM1, MM0			// UnPair			// (P ^ Dst)

			PAND		MM1, MM3			// UnPair			// () & mask

			PXOR		MM0, MM1								// P ^ ()
			ADD			EDI, 8

			ADD			ESI, 8				// AGI���

			MOVQ		[EDI-8], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_5

		LoopX_5: // 18�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// 1// UnPair

			MOVQ		MM3, [ESI+8]		// 2
			MOVQ		MM2, MM0			// 1

			MOVQ		MM1, [EDI]			// 1
			MOVQ		MM5, MM3			// 2

			MOVQ		MM4, [EDI+8]		// 2
			PAND		MM0, MM1			// 1

			PAND		MM3, MM4			// 2
			PXOR		MM2, MM1			// 1

			PXOR		MM5, MM4			// 2
			PSRLD		MM2, 1				// 1

			PSRLD		MM5, 1				// 2
			PAND		MM2, MM6			// 1

			PAND		MM5, MM6			// 2
			PADDD		MM0, MM2			// 1

			MOVQ		MM2, [ESI]			// 1
			PADDD		MM3, MM5			// 2

			MOVQ		MM5, [ESI+8]		// 2
			PCMPEQD		MM2, MM7			// 1				//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0

			PCMPEQD		MM5, MM7			// 2				//	�����F��Src�̔�r���True:MM3=0xffffffffffffffff False:MM3=0
			PXOR		MM1, MM0			// 1				// (P ^ Dst)

			PXOR		MM4, MM3			// 2				// (P ^ Dst)
			PAND		MM1, MM2			// 1				// () & mask

			PAND		MM4, MM5			// 2				// () & mask
			PXOR		MM0, MM1			// 1				// P ^ ()

			PXOR		MM3, MM4			// 2				// P ^ ()
			ADD			ESI, 16

			MOVQ		[EDI], MM0			// 1// UnPair

			MOVQ		[EDI+8], MM3		// 2// UnPair

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_5				// UnPair

		EndLoop_5:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_5				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:�L	 1:1�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;


		_asm
		{
			MOVD		MM6, Mask7f
			MOV			EDX, nHeight

			MOVQ		MM1, MM6
			MOV			ECX, nWidth

			PUNPCKLDQ	MM6, MM1								// 0x007f7f7f
			MOVD		MM7, colKey								// �J���[�L�[��ݒ肷��

			MOV			EDI, lpDst
			MOVQ		MM1, MM7

			MOV			ESI, lpSrc
			PUNPCKLDQ	MM7, MM1								// ColKey

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;

		LoopY_6:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_6: // 15�N���b�N��T�C�N��
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM1, [EDI]
			MOVQ		MM3, MM0

			PCMPEQD		MM3, MM7								//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0
			MOVQ		MM2, MM0

			PAND		MM0, MM1
			PXOR		MM2, MM1

			PSRLD		MM2, 1				// UnPair

			PAND		MM2, MM6			// UnPair

			PADDD		MM0, MM2			// UnPair

			PXOR		MM1, MM0			// UnPair			// (P ^ Dst)

			PAND		MM1, MM3								// () & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM0, MM1								// P ^ ()
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_6									// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_6:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_6

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_6									// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l
		SkipY_6:
			DEC			EDX
			JNZ			LoopY_6

			EMMS
		}
	} // if

	
	return 0;
} // BlendBltHalf


//////////////////////////////////////
//	BlendBltFastHalf
//	�����F������1�F1�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BlendBltFastHalf(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BlendBltFastHalf��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::BlendBltFastHalf��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);			// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:��	 1:1�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	Mask7f = 0x007f7f7f;


		_asm
		{
			MOVD		MM6, Mask7f
			MOV			EDX, nHeight

			MOVQ		MM1, MM6
			MOV			ECX, nWidth

			PUNPCKLDQ	MM6, MM1								// 0x007f7f7f

			MOV			ESI, lpSrc
			MOV			EDI, lpDst

		LoopY_7:
			SHR			ECX, 1
			JNB			Skip_7									//	�������ۂ��̔���
			// Step1
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM1, [EDI]
			MOVQ		MM2, MM0

			MOVQ		MM3, MM1
			PAND		MM0, MM1

			PXOR		MM2, MM3			// UnPair

			PSRLD		MM2, 1				// UnPair

			PAND		MM2, MM6			// UnPair

			PADDD		MM0, MM2
			ADD			EDI, 4

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_7			// UnPair

		Skip_7:
			SHR			ECX, 1
			JNB			LoopX_7								//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			MOVQ		MM1, [EDI]
			MOVQ		MM2, MM0

			MOVQ		MM3, MM1
			PAND		MM0, MM1

			PXOR		MM2, MM3			// UnPair

			PSRLD		MM2, 1				// UnPair

			PAND		MM2, MM6			// UnPair

			PADDD		MM0, MM2
			ADD			EDI, 8

			ADD			ESI, 8				// AGI���

			MOVQ		[EDI-8], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_7

		LoopX_7: // 13�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// 1// UnPair

			MOVQ		MM3, [ESI+8]		// 2
			MOVQ		MM2, MM0			// 1

			MOVQ		MM1, [EDI]			// 1
			MOVQ		MM4, MM3			// 2

			MOVQ		MM5, [EDI+8]		// 2
			PAND		MM0, MM1			// 1

			PAND		MM3, MM5			// 2
			PXOR		MM2, MM1			// 1

			PXOR		MM4, MM5			// 2
			PSRLD		MM2, 1				// 1

			PSRLD		MM4, 1				// 2
			PAND		MM2, MM6			// 1

			PAND		MM4, MM6			// 2
			PADDD		MM0, MM2			// 1

			PADDD		MM3, MM4			// 2
			ADD			ESI, 16

			MOVQ		[EDI], MM0			// 1// UnPair

			MOVQ		[EDI+8], MM3		// 2// UnPair

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_7			// UnPair

		EndLoop_7:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_7			// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:�L	 1:1�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOVD		MM6, Mask7f
			MOV			EDX, nHeight

			MOVQ		MM1, MM6
			MOV			ECX, nWidth

			PUNPCKLDQ	MM6, MM1								// 0x007f7f7f
			MOV			EDI, lpDst

			MOV			ESI, lpSrc
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_8:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_8: // 12�N���b�N��T�C�N��
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM1, [EDI]
			MOVQ		MM2, MM0

			MOVQ		MM3, MM1
			PAND		MM0, MM1

			PXOR		MM2, MM3			// UnPair

			PSRLD		MM2, 1				// UnPair

			PAND		MM2, MM6					
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PADDD		MM0, MM2
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_8									// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_8:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_8

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_8									// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l

		SkipY_8:
			DEC			EDX
			JNZ			LoopY_8

			EMMS
		}
	} // if


	return 0;
} // BlendBltFastHalf


//////////////////////////////////////
//	BlendBlt
//	�����F�L���RGB�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BlendBlt(CDIB32* lpDIB32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
						   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BlendBlt��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::BlendBlt��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);										// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:��	 RGB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			EDI, lpDst

			MOVQ		MM1, MM7
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM7, MM1			// UnPair			// ColKey
			PXOR		MM0, MM0

			PXOR		MM1, MM1
			PXOR		MM4, MM4

			MOVD		MM2, dwSrcRGBRate
			PUNPCKLBW	MM2, MM4								// WORD�P�ʂŏ�Z����̂�

			MOVD		MM3, dwDstRGBRate
			PUNPCKLBW	MM3, MM4

		LoopY_9:
			SHR			ECX, 1
			JNB			LoopX_9								//	�������ۂ��̔���
			// Step1
			MOVD		MM0, [ESI]			// 1// UnPair

			MOVD		MM1, [EDI]
			MOVQ		MM5, MM0

			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�
			MOVQ		MM6, MM1

			PUNPCKLBW	MM1, MM4
			PMULLW		MM0, MM2

			PMULLW		MM1, MM3
			PCMPEQD		MM5, MM7								//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4			// UnPair

			PXOR		MM6, MM0			// UnPair			// (P ^ Dst)

			PAND		MM6, MM5			// UnPair			// & mask

			PXOR		MM0, MM6								// P ^ ()
			ADD			EDI, 4

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM0

			OR			ECX, ECX
			JZ			EndLoop_9

		LoopX_9: // 22�N���b�N��T�C�N��(��Z�� 3�N���b�N�� 30�N���b�N)
			// Step2
			MOVD		MM0, [ESI]			// 1// UnPair

			MOVD		MM5, [ESI+4]		// 2
			PUNPCKLBW	MM0, MM4			// 1				// WORD�P�ʂŏ�Z����̂�

			MOVD		MM1, [EDI]			// 1
			PUNPCKLBW	MM5, MM4			// 2				// WORD�P�ʂŏ�Z����̂�

			PMULLW		MM0, MM2			// 1
			PUNPCKLBW	MM1, MM4			// 1

			MOVD		MM6, [EDI+4]		// 2
			PMULLW		MM1, MM3			// 1

			PUNPCKLBW	MM6, MM4			// 2
			PMULLW		MM5, MM2			// 2

			PMULLW		MM6, MM3			// 2
			NOP			// PMUL�X�g�[�����

			PADDUSB		MM0, MM1			// 1// UnPair

			PSRLW		MM0, 8				// 1// UnPair

			PACKUSWB	MM0, MM4			// 1
			PADDUSB		MM5, MM6			// 2

			MOVD		MM6, [ESI]			// 1
			PSRLW		MM5, 8				// 2

			MOVD		MM1, [EDI]			// 1
			PACKUSWB	MM5, MM4			// 2

			PCMPEQD		MM6, MM7			// 1				//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0
			PXOR		MM1, MM0			// 1				// (P ^ Dst)

			PAND		MM1, MM6			// 1//UnPair		// & mask

			MOVD		MM6, [ESI+4]		// 2
			PXOR		MM0, MM1			// 1				// P ^ ()

			MOVD		MM1, [EDI+4]		// 2
			PCMPEQD		MM6, MM7			// 2				//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0

			PXOR		MM1, MM5			// 2// UnPair		// (P ^ Dst)

			PAND		MM1, MM6			// 2				// & mask
			ADD			EDI, 8

			PXOR		MM5, MM1			// 2				// P ^ ()
			ADD			ESI, 8

			MOVD		[EDI-8], MM0		// 1// UnPair

			MOVD		[EDI-4], MM5		// 2// UnPair

			DEC			ECX
			JNZ			LoopX_9

		EndLoop_9:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_9				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:�L	 RGB�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;


		_asm
		{
			MOVD		MM6, Mask7f
			MOV			EDX, nHeight

			MOVQ		MM1, MM6
			MOV			ECX, nWidth

			PUNPCKLDQ	MM6, MM1								// 0x007f7f7f
			MOVD		MM7, colKey								// �J���[�L�[��ݒ肷��

			MOV			EDI, lpDst
			MOVQ		MM1, MM7

			MOV			ESI, lpSrc
			PUNPCKLDQ	MM7, MM1								// ColKey

			PXOR		MM0, MM0
			PXOR		MM1, MM1

			MOVD		MM2, dwSrcRGBRate
			PXOR		MM4, MM4

			MOVD		MM3, dwDstRGBRate
			PUNPCKLBW	MM2, MM4								// WORD�P�ʂŏ�Z����̂�

			PUNPCKLBW	MM3, MM4
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_10:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_10: // 18�N���b�N��T�C�N��(��Z�� 3�N���b�N�� 22�N���b�N)
			MOVD		MM0, [ESI]			// 1// UnPair

			MOVD		MM1, [EDI]
			MOVQ		MM5, MM0

			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�
			MOVQ		MM6, MM1

			PUNPCKLBW	MM1, MM4
			PMULLW		MM0, MM2

			PMULLW		MM1, MM3
			PCMPEQD		MM5, MM7								//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4			// UnPair

			PXOR		MM6, MM0			// UnPair			// (P ^ Dst)

			PAND		MM6, MM5								// & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM0, MM6								// P ^ ()
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_10								// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_10:
			MOVD		[EDI-4], MM0

			DEC			ECX
			JNZ			LoopX_10

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_10								// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l
		SkipY_10:
			DEC			EDX
			JNZ			LoopY_10

			EMMS
		}
	} // if

	
	return 0;
} // BlendBlt


//////////////////////////////////////
//	BlendBltFast
//	�����F������RGB�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BlendBltFast(CDIB32* lpDIB32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
								 ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BlendBltFast��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::BlendBltFast��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);										// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:��	 RGB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			PXOR		MM0, MM0
			PXOR		MM1, MM1

			MOVD		MM2, dwSrcRGBRate
			PXOR		MM4, MM4

			MOVD		MM3, dwDstRGBRate
			PUNPCKLBW	MM2, MM4								// WORD�P�ʂŏ�Z����̂�

			PUNPCKLBW	MM3, MM4			// UnPair

		LoopY_11:
			SHR			ECX, 1
			JNB			LoopX_11								//	�������ۂ��̔���
			// Step1
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM1, [EDI]
			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�

			PUNPCKLBW	MM1, MM4
			PMULLW		MM0, MM2

			PMULLW		MM1, MM3
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4
			ADD			EDI, 4

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_11

		LoopX_11: // 16�N���b�N��T�C�N��(��Z�� 3�N���b�N�� 24�N���b�N)
			// Step2
			MOVD		MM0, [ESI]			// 1// UnPair

			MOVD		MM5, [ESI+4]		// 2
			PUNPCKLBW	MM0, MM4			// 1				// WORD�P�ʂŏ�Z����̂�

			MOVD		MM1, [EDI]			// 1
			PMULLW		MM0, MM2			// 1

			MOVD		MM6, [EDI+4]		// 2
			PUNPCKLBW	MM1, MM4			// 1

			PMULLW		MM1, MM3			// 1// UnPair
			PUNPCKLBW	MM5, MM4			// 2				// WORD�P�ʂŏ�Z����̂�

			PMULLW		MM5, MM2			// 2
			PUNPCKLBW	MM6, MM4			// 2

			PMULLW		MM6, MM3			// 2
			NOP			// PMUL�X�g�[�����

			PADDUSB		MM0, MM1			// 1// UnPair

			PSRLW		MM0, 8				// 1// UnPair

			PACKUSWB	MM0, MM4			// 1
			PADDUSB		MM5, MM6			// 2

			PSRLW		MM5, 8				// 2// UnPair

			PACKUSWB	MM5, MM4			// 2
			ADD			ESI, 8

			MOVD		[EDI], MM0			// 1

			MOVD		[EDI+4], MM5		// 2// UnPair

			ADD			EDI, 8
			DEC			ECX

			JNZ			LoopX_11			// UnPair

		EndLoop_11:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_11			// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:�L	 RGB�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOVD		MM6, Mask7f
			MOV			EDX, nHeight

			MOVQ		MM1, MM6
			MOV			ECX, nWidth

			PUNPCKLDQ	MM6, MM1								// 0x007f7f7f
			MOV			EDI, lpDst

			MOV			ESI, lpSrc
			PXOR		MM0, MM0

			PXOR		MM1, MM1
			PXOR		MM4, MM4

			MOVD		MM2, dwSrcRGBRate
			MOV			EBX, EIY								// nEyCnt = EIY;

			MOVD		MM3, dwDstRGBRate
			PUNPCKLBW	MM2, MM4								// WORD�P�ʂŏ�Z����̂�

			PUNPCKLBW	MM3, MM4			// UnPair

		LoopY_12:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_12: // 14�N���b�N��T�C�N��(��Z�� 3�N���b�N�� 18�N���b�N)
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM1, [EDI]
			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�

			PUNPCKLBW	MM1, MM4
			PMULLW		MM0, MM2

			PMULLW		MM1, MM3
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PACKUSWB	MM0, MM4
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_12								// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_12:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_12

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_12								// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l

		SkipY_12:
			DEC			EDX
			JNZ			LoopY_12

			EMMS
		}
	} // if

	
	return 0;
} // BlendBltFast


//////////////////////////////////////
//	AddColorBlt
//	�����F�L���ADD�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::AddColorBlt(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::AddColorBlt��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::AddColorBlt��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);										// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2)+m_rcDstRect.top*	m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:��	 ADD�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			EDI, lpDst

			MOVQ		MM1, MM7
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM7, MM1			// UnPair			// ColKey

		LoopY_13:
			SHR			ECX, 1
			JNB			Skip_13									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM2, [EDI]
			MOVQ		MM1, MM0

			PADDUSB		MM0, MM2
			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000

			PXOR		MM2, MM0			// UnPair			// (P ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// P ^ ()
			ADD			EDI, 4

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_13

		Skip_13:
			SHR			ECX, 1
			JNB			LoopX_13								//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			MOVQ		MM2, [EDI]
			MOVQ		MM1, MM0

			PADDUSB		MM0, MM2
			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000

			PXOR		MM2, MM0			// UnPair			// (P ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// P ^ ()
			ADD			EDI, 8

			ADD			ESI, 8				// AGI���

			MOVQ		[EDI-8], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_13

		LoopX_13: // 13�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// 1//UnPair

			MOVQ		MM3, [ESI+8]		// 2
			MOVQ		MM1, MM0			// 1

			MOVQ		MM2, [EDI]			// 1
			MOVQ		MM5, MM3			// 2

			MOVQ		MM4, [EDI+8]		// 2
			PADDUSB		MM0, MM2			// 1

			PADDUSB		MM3, MM4			// 2
			PXOR		MM2, MM0			// 1				// (src ^ dst)

			PXOR		MM4, MM3			// 2				// (src ^ dst)
			PCMPEQD		MM1, MM7			// 1				// ColKey:0xffffffff NonColKey:0x00000000

			PCMPEQD		MM5, MM7			// 2				// ColKey:0xffffffff NonColKey:0x00000000
			PAND		MM2, MM1			// 1				// & mask

			PAND		MM4, MM5			// 2				// & mask
			PXOR		MM0, MM2			// 1				// src ^ ()

			PXOR		MM3, MM4			// 2				// src ^ ()
			ADD			EDI, 16

			ADD			ESI, 16				// AGI���

			MOVQ		[EDI-16], MM0		// 1// UnPair

			MOVQ		[EDI-8], MM3		// 2// UnPair
			
			DEC			ECX
			JNZ			LoopX_13

		EndLoop_13:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_13			// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:�L	 ADD�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	nAddPixel = 1 << 2;	   // �{���̓~���[�ƕ������̂Ő��l�𒼏����ł���񂾂���ǁE�E�E�܁A������:p
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;


		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			PXOR		MM1, MM1

			MOVQ		MM2, MM7
			PXOR		MM0, MM0

			PUNPCKLDQ	MM7, MM2								// ColKey
			MOV			ECX, nWidth

			MOV			EDX, nHeight
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_14:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_14: // 11�N���b�N��T�C�N��
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM2, [EDI]
			MOVQ		MM1, MM0

			PADDUSB		MM0, MM2
			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000

			PXOR		MM2, MM0			// UnPair			// (P ^ dst)

			PAND		MM2, MM1								// & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM0, MM2								// P ^ ()
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_14								// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_14:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_14

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_14								// if ( EY >= 0 )

			ADD			ESI, lPitchSrc					// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l

		SkipY_14:
			DEC			EDX
			JNZ			LoopY_14

			EMMS
		}
	} // if

	
	return 0;
} // AddColorBlt


//////////////////////////////////////
//	AddColorBltFast
//	�����F������ADD�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::AddColorBltFast(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::AddColorBltFast��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::AddColorBltFast��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);										// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2)+m_rcDstRect.top*	m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:��	 ADD�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			ESI, lpSrc
			MOV			EDI, lpDst

		LoopY_15:
			SHR			ECX, 1
			JNB			Skip_15									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM2, [EDI]			// UnPair

			PADDUSB		MM0, MM2
			ADD			EDI, 4

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_15

		Skip_15:
			SHR			ECX, 1
			JNB			LoopX_15								//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			MOVQ		MM2, [EDI]			// UnPair

			PADDUSB		MM0, MM2
			ADD			EDI, 8

			ADD			ESI, 8				// AGI���

			MOVQ		[EDI-8], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_15

		LoopX_15: // 9�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// 1// UnPair

			MOVQ		MM2, [EDI]			// 1// UnPair

			MOVQ		MM3, [ESI+8]		// 2
			PADDUSB		MM0, MM2			// 1

			MOVQ		MM5, [EDI+8]		// 2// UnPair

			PADDUSB		MM3, MM5			// 2
			ADD			ESI, 16

			MOVQ		[EDI], MM0			// UnPair

			MOVQ		[EDI+8], MM3		// UnPair

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_15			// UnPair

		EndLoop_15:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_15			// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:�L	 ADD�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	nAddPixel = 1 << 2;	   // �{���̓~���[�ƕ������̂Ő��l�𒼏����ł���񂾂���ǁE�E�E�܁A������:p
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;


		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			PXOR		MM1, MM1
			PXOR		MM0, MM0

			MOV			ECX, nWidth
			MOV			EDX, nHeight

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;

		LoopY_16:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_16: // 8�N���b�N��T�C�N��
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM2, [EDI]			// UnPair

			PADDUSB		MM0, MM2
			ADD			ESI, AddSrcPixel						// �������̉��Z

			ADD			EDI, 4
			ADD			EAX, EX									// EX += 2*DX;

			JNB			SkipX_16			// UnPair			// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_16:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_16

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
	
			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_16								// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l

		SkipY_16:
			DEC			EDX
			JNZ			LoopY_16

			EMMS
		}
	} // if


	return 0;
} // AddColorBltFast


//////////////////////////////////////
//	SubColorBlt
//	�����F�L���SUB�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::SubColorBlt(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::SubColorBlt��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::SubColorBlt��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);										// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2)+m_rcDstRect.top*	m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:��	 SUB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			EDI, lpDst

			MOVQ		MM1, MM7
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM7, MM1			// UnPair			// ColKey

		LoopY_17:
			SHR			ECX, 1
			JNB			Skip_17									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM2, [EDI]			// UnPair

			MOVQ		MM1, MM2
			PSUBUSB		MM2, MM0

			PCMPEQD		MM0, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM1, MM2								// (P ^ dst)

			PAND		MM1, MM0			// UnPair			// & mask

			PXOR		MM2, MM1								// P ^ ()
			ADD			EDI, 4

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM2		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_17

		Skip_17:
			SHR			ECX, 1
			JNB			LoopX_17									//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			MOVQ		MM2, [EDI]			// UnPair

			MOVQ		MM1, MM2
			PSUBUSB		MM2, MM0

			PCMPEQD		MM0, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM1, MM2								// (P ^ dst)

			PAND		MM1, MM0			// UnPair			// & mask

			PXOR		MM2, MM1								// P ^ ()
			ADD			EDI, 8

			ADD			ESI, 8				// AGI���

			MOVQ		[EDI-8], MM2		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_17

		LoopX_17: // 13�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// 1// UnPair

			MOVQ		MM2, [EDI]			// 1// UnPair

			MOVQ		MM3, [ESI+8]		// 2
			MOVQ		MM1, MM2			// 1

			MOVQ		MM4, [EDI+8]		// 2
			PSUBUSB		MM2, MM0			// 1

			MOVQ		MM5, MM4			// 2
			PSUBUSB		MM4, MM3			// 2

			PCMPEQD		MM0, MM7			// 1				// ColKey:0xffffffff NonColKey:0x00000000
			PCMPEQD		MM3, MM7			// 2				// ColKey:0xffffffff NonColKey:0x00000000

			PXOR		MM1, MM2			// 1				// (P ^ dst)
			PXOR		MM5, MM4			// 2				// (P ^ dst)

			PAND		MM1, MM0			// 1				// & mask
			PAND		MM5, MM3			// 2				// & mask

			PXOR		MM2, MM1			// 1				// P ^ ()
			ADD			EDI, 16

			PXOR		MM4, MM5			// 2				// P ^ ()
			ADD			ESI, 16

			MOVQ		[EDI-16], MM2		// 1// UnPair

			MOVQ		[EDI-8], MM4		// 2// UnPair

			DEC			ECX
			JNZ			LoopX_17

		EndLoop_17:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_17			// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:�L	 SUB�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	nAddPixel = 1 << 2;	   // �{���̓~���[�ƕ������̂Ő��l�𒼏����ł���񂾂���ǁE�E�E�܁A������:p
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			PXOR		MM1, MM1

			MOVQ		MM2, MM7
			PXOR		MM0, MM0

			PUNPCKLDQ	MM7, MM2								// ColKey
			MOV			ECX, nWidth

			MOV			EDX, nHeight
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_18:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_18: // 11�N���b�N��T�C�N��
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM2, [EDI]			// UnPair

			MOVQ		MM1, MM2
			PSUBUSB		MM2, MM0

			PCMPEQD		MM0, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM1, MM2								// (P ^ dst)

			PAND		MM1, MM0								// & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM2, MM1								// P ^ ()
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_18								// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_18:
			MOVD		[EDI-4], MM2		// UnPair

			DEC			ECX
			JNZ			LoopX_18

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_18								// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l

		SkipY_18:
			DEC			EDX
			JNZ			LoopY_18

			EMMS
		}
	} // if

	
	return 0;
} // SubColorBlt


//////////////////////////////////////
//	SubColorBltFast
//	�����F������SUB�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::SubColorBltFast(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::SubColorBltFast��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::SubColorBltFast��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);										// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2)+m_rcDstRect.top*	m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:��	 SUB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			ESI, lpSrc
			MOV			EDI, lpDst

		LoopY_19:
			SHR			ECX, 1
			JNB			Skip_19								//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM2, [EDI]			// UnPair

			PSUBUSB		MM2, MM0
			ADD			EDI, 4

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM2		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_19

		Skip_19:
			SHR			ECX, 1
			JNB			LoopX_19								//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			MOVQ		MM2, [EDI]			// UnPair

			PSUBUSB		MM2, MM0
			ADD			EDI, 8

			ADD			ESI, 8				// AGI���

			MOVQ		[EDI-8], MM2		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_19

		LoopX_19: // 9�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// 1// UnPair

			MOVQ		MM2, [EDI]			// 1// UnPair

			MOVQ		MM3, [ESI+8]		// 2
			PSUBUSB		MM2, MM0			// 1

			MOVQ		MM5, [EDI+8]		// 2// UnPair

			PSUBUSB		MM5, MM3			// 2
			ADD			ESI, 16

			MOVQ		[EDI], MM2			// UnPair

			MOVQ		[EDI+8], MM5		// UnPair

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_19			// UnPair

		EndLoop_19:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_19			// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:�L	 SUB�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	nAddPixel = 1 << 2;	   // �{���̓~���[�ƕ������̂Ő��l�𒼏����ł���񂾂���ǁE�E�E�܁A������:p
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			PXOR		MM1, MM1
			PXOR		MM0, MM0

			MOV			ECX, nWidth
			MOV			EDX, nHeight

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;

		LoopY_20:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_20: // 8�N���b�N��T�C�N��
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM2, [EDI]			// UnPair

			PSUBUSB		MM2, MM0
			ADD			ESI, AddSrcPixel						// �������̉��Z

			ADD			EDI, 4				// UnPair

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_20								// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_20:
			MOVD		[EDI-4], MM2		// UnPair

			DEC			ECX
			JNZ			LoopX_20

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_20								// if ( EY >= 0 )

			ADD			ESI, lPitchSrc					// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l

		SkipY_20:
			DEC			EDX
			JNZ			LoopY_20

			EMMS
		}
	} // if


	return 0;
} // SubColorBltFast


////////////////////////////////////////////////////////////////////
//	�~���[�L���`�]��
////////////////////////////////////////////////////////////////////
//////////////////////////////////////
//	Blt
//	�����F�L��̓]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BltM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BltM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::BltM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			EDI, lpDst

			MOVQ		MM0, MM7
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM7, MM0			// UnPair			// ColKey

		LoopY_1:
			SHR			ECX, 1
			JNB			Skip_1									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [EDI]
			MOVQ		MM1, MM0

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// src ^ ()
			ADD			EDI, 4

			ADD			ESI, -4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_1

		Skip_1:
			SHR			ECX, 1
			JNB			LoopX_1									//	4�̔{�����ۂ��̔���
			// Step2
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [ESI-8]		// UnPair

			MOVQ		MM2, [EDI]
			PUNPCKLDQ	MM0, MM1								// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM1, MM0			// UnPair

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// src ^ ()
			ADD			EDI, 8

			ADD			ESI, -8				// AGI���

			MOVQ		[EDI-8], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_1

		LoopX_1: // 15�N���b�N��T�C�N��
			// Step4
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM1, [ESI-8]		// 1// UnPair

			MOVD		MM3, [ESI-12]		// 2
			PUNPCKLDQ	MM0, MM1			// 1				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVD		MM4, [ESI-16]		// 2
			MOVQ		MM1, MM0			// 1

			MOVQ		MM2, [EDI]			// 1
			PUNPCKLDQ	MM3, MM4			// 2				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM5, [EDI+8]		// 2
			MOVQ		MM4, MM3			// 2

			PCMPEQD		MM1, MM7			// 1				// ColKey:0xffffffff NonColKey:0x00000000
			PCMPEQD		MM4, MM7			// 2				// ColKey:0xffffffff NonColKey:0x00000000

			PXOR		MM2, MM0			// 1				// (src ^ dst)
			PXOR		MM5, MM3			// 2				// (src ^ dst)

			PAND		MM2, MM1			// 1				// & mask
			PAND		MM5, MM4			// 2				// & mask

			PXOR		MM0, MM2			// 1				// src ^ ()
			PXOR		MM3, MM5			// 2				// src ^ ()

			ADD			ESI, -16			// AGI���

			MOVQ		[EDI], MM0			// 1// UnPair

			MOVQ		[EDI+8], MM3		// 2// UnPair

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_1				// UnPair

		EndLoop_1:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_1				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			ECX, nWidth

			MOVQ		MM0, MM7
			MOV			EDX, nHeight

			PUNPCKLDQ	MM7, MM0								// ColKey
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_2:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_2: // 10�N���b�N��T�C�N��
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [EDI]
			MOVQ		MM1, MM0

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1								// & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM0, MM2								// src ^ ()
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_2									// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_2:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_2

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_2										// if ( EY >= 0 )

			ADD			ESI, lPitchSrc
			SUB			EBX, EY2									// Y�̕␳

		SkipY_2:
			DEC			EDX
			JNZ			LoopY_2

			EMMS
		}
	} // if


	return 0;
} // BltM


//////////////////////////////////////
//	BltFastM
//	�����F�����̓]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BltFastM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BltFastM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::BltFastM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����

// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

		LoopY_3:
			SHR			ECX, 1
			JNB			Skip_3									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair

			ADD			ESI, -4				// AGI���

			MOVD		[EDI], MM0			// UnPair

			ADD			EDI, 4
			OR			ECX, ECX

			JZ			EndLoop_3			// UnPair

		Skip_3:
			SHR			ECX, 1
			JNB			LoopX_3									//	4�̔{�����ۂ��̔���
			// Step2
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [ESI-8]		// UnPair

			PUNPCKLDQ	MM0, MM1								// �\�[�X�� 2�s�N�Z�����~���[����
			ADD			EDI, 8

			ADD			ESI, -8				// AGI���

			MOVQ		[EDI], MM0			// UnPair

			OR			ECX, ECX
			JZ			EndLoop_3

		LoopX_3: // 9�N���b�N��T�C�N��							// ����������ƃA�����[�����������ǁE�E�E�܁A������:p
			// Step4
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM1, [ESI-8]		// 1// UnPair

			MOVD		MM3, [ESI-12]		// 2
			PUNPCKLDQ	MM0, MM1			// 1				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVD		MM4, [ESI-16]		// 2// UnPair

			PUNPCKLDQ	MM3, MM4			// 2				// �\�[�X�� 2�s�N�Z�����~���[����
			ADD			ESI, -16

			MOVQ		[EDI], MM0			// 1// UnPair

			MOVQ		[EDI+8], MM3		// 2// UnPair

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_3				// UnPair

		EndLoop_3:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_3				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:�L	 --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			MOV			ECX, nWidth
			MOV			EDX, nHeight

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;

		LoopY_4:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_4: // 7�N���b�N��T�C�N��
			MOVD		MM0, [ESI-4]		// UnPair

			ADD			ESI, AddSrcPixel						// �������̉��Z
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_4									// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_4:
			MOVD		[EDI-4], MM0			// UnPair

			DEC			ECX
			JNZ			LoopX_4

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_4										// if ( EY >= 0 )

			SUB			EBX, EY2									// Y�̕␳
			ADD			ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

		SkipY_4:
			DEC			EDX
			JNZ			LoopY_4

			EMMS
		}
	} // if


	return 0;
} // BltFastM


//////////////////////////////////////
//	BlendBltHalfM
//	�����F�L��̂P�F�P�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BlendBltHalfM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BlendBltHalfM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::BlendBltHalfM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:��	 1:1�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;
		DWORD	Mask7f = 0x007f7f7f;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOVD		MM6, Mask7f
			MOV			EDX, nHeight

			MOVQ		MM0, MM6
			MOV			ECX, nWidth

			PUNPCKLDQ	MM6, MM0								// 0x007f7f7f
			MOVD		MM7, colKey								// �J���[�L�[��ݒ肷��

			MOV			EDI, lpDst
			MOVQ		MM0, MM7

			MOV			ESI, lpSrc
			PUNPCKLDQ	MM7, MM0								// ColKey

		LoopY_5:
			SHR			ECX, 1
			JNB			Skip_5									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [EDI]
			MOVQ		MM3, MM0

			PCMPEQD		MM3, MM7								//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0
			MOVQ		MM2, MM0

			PAND		MM0, MM1
			PXOR		MM2, MM1

			PSRLD		MM2, 1				// UnPair

			PAND		MM2, MM6			// UnPair

			PADDD		MM0, MM2			// UnPair

			PXOR		MM1, MM0			// UnPair			// (P ^ Dst)

			PAND		MM1, MM3			// UnPair			// () & mask

			PXOR		MM0, MM1								// P ^ ()
			ADD			EDI, 4

			ADD			ESI, -4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_5

		Skip_5:
			SHR			ECX, 1
			JNB			LoopX_5									//	4�̔{�����ۂ��̔���
			// Step2
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [ESI-8]		// UnPair

			MOVQ		MM1, [EDI]
			PUNPCKLDQ	MM0, MM2								// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM3, MM0			// UnPair

			PCMPEQD		MM3, MM7								//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0
			MOVQ		MM2, MM0

			PAND		MM0, MM1
			PXOR		MM2, MM1

			PSRLD		MM2, 1				// UnPair

			PAND		MM2, MM6			// UnPair

			PADDD		MM0, MM2			// UnPair

			PXOR		MM1, MM0			// UnPair			// (P ^ Dst)

			PAND		MM1, MM3			// UnPair			// () & mask

			PXOR		MM0, MM1								// P ^ ()
			ADD			EDI, 8

			ADD			ESI, -8				// AGI���

			MOVQ		[EDI-8], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_5

		LoopX_5: // 24�N���b�N��T�C�N��
			// Step4
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM2, [ESI-8]		// 1// UnPair

			MOVD		MM3, [ESI-12]		// 2
			PUNPCKLDQ	MM0, MM2			// 1				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVD		MM5, [ESI-16]		// 2// UnPair

			MOVQ		MM1, [EDI]			// 1
			PUNPCKLDQ	MM3, MM5			// 2				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM4, [EDI+8]		// 2
			MOVQ		MM2, MM0			// 1

			MOVQ		MM5, MM3			// 2
			PAND		MM0, MM1			// 1

			PAND		MM3, MM4			// 2
			PXOR		MM2, MM1			// 1

			PXOR		MM5, MM4			// 2
			PSRLD		MM2, 1				// 1

			PSRLD		MM5, 1				// 2
			PAND		MM2, MM6			// 1

			PAND		MM5, MM6			// 2
			PADDD		MM0, MM2			// 1

			MOVD		MM2, [ESI-4]		// 1
			PADDD		MM3, MM5			// 2

			MOVD		MM5, [ESI-8]		// 1// UnPair

			PUNPCKLDQ	MM2, MM5			// 1				// �\�[�X�� 2�s�N�Z�����~���[����
			PXOR		MM1, MM0			// 1				// (P ^ Dst)

			MOVD		MM5, [ESI-12]		// 2
			PCMPEQD		MM2, MM7			// 1				//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0

			PAND		MM1, MM2			// 1				// () & mask
			PXOR		MM4, MM3			// 2				// (P ^ Dst)

			MOVD		MM2, [ESI-16]		// 2
			PXOR		MM0, MM1			// 1				// P ^ ()

			PUNPCKLDQ	MM5, MM2			// 2// UnPair		// �\�[�X�� 2�s�N�Z�����~���[����

			PCMPEQD		MM5, MM7			// 2// UnPair		//	�����F��Src�̔�r���True:MM3=0xffffffffffffffff False:MM3=0
			
			PAND		MM4, MM5			// 2				// () & mask
			ADD			EDI, 16

			PXOR		MM3, MM4			// 2				// P ^ ()
			ADD			ESI, -16

			MOVQ		[EDI-16], MM0		// 1// UnPair

			MOVQ		[EDI-8], MM3		// 2// UnPair
			
			DEC			ECX
			JNZ			LoopX_5

		EndLoop_5:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_5				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 1:1�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOVD		MM6, Mask7f
			MOV			EDX, nHeight

			MOVQ		MM0, MM6
			MOV			ECX, nWidth

			PUNPCKLDQ	MM6, MM0								// 0x007f7f7f
			MOVD		MM7, colKey								// �J���[�L�[��ݒ肷��

			MOV			EDI, lpDst
			MOVQ		MM0, MM7

			MOV			ESI, lpSrc
			PUNPCKLDQ	MM7, MM0								// ColKey

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;

		LoopY_6:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_6: // 15�N���b�N��T�C�N��
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [EDI]
			MOVQ		MM3, MM0

			PCMPEQD		MM3, MM7								//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0
			MOVQ		MM2, MM0

			PAND		MM0, MM1
			PXOR		MM2, MM1

			PSRLD		MM2, 1				// UnPair

			PAND		MM2, MM6			// UnPair

			PADDD		MM0, MM2			// UnPair

			PXOR		MM1, MM0			// UnPair			// (P ^ Dst)

			PAND		MM1, MM3								// () & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM0, MM1								// P ^ ()
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_6									// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_6:
			MOVD		[EDI-4], MM0			// UnPair

			DEC			ECX
			JNZ			LoopX_6

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_6										// if ( EY >= 0 )

			SUB			EBX, EY2									// Y�̕␳
			ADD			ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

		SkipY_6:
			DEC			EDX
			JNZ			LoopY_6

			EMMS
		}
	} // if


	return 0;
} // BlendBltHalfM


//////////////////////////////////////
//	BlendBltFastHalfM
//	�����F�����̂P�F�P�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BlendBltFastHalfM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BlendBltFastHalfM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::BlendBltFastHalfM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:��	 1:1�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	Mask7f = 0x007f7f7f;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOVD		MM6, Mask7f
			MOV			EDX, nHeight

			MOVQ		MM0, MM6
			MOV			ECX, nWidth

			PUNPCKLDQ	MM6, MM0								// 0x007f7f7f
			MOV			EDI, lpDst

			MOV			ESI, lpSrc			// UnPair

		LoopY_7:
			SHR			ECX, 1
			JNB			Skip_7									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [EDI]
			MOVQ		MM2, MM0

			PAND		MM0, MM1
			PXOR		MM2, MM1

			PSRLD		MM2, 1				// UnPair

			PAND		MM2, MM6			// UnPair

			PADDD		MM0, MM2
			ADD			EDI, 4

			ADD			ESI, -4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_7

		Skip_7:
			SHR			ECX, 1
			JNB			LoopX_7									//	4�̔{�����ۂ��̔���
			// Step2
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [ESI-8]		// UnPair

			MOVQ		MM1, [EDI]
			PUNPCKLDQ	MM0, MM2								// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM2, MM0			// UnPair
			PAND		MM0, MM1

			PXOR		MM2, MM1			// UnPair

			PSRLD		MM2, 1				// UnPair

			PAND		MM2, MM6			// UnPair

			PADDD		MM0, MM2
			ADD			EDI, 8

			ADD			ESI, -8				// AGI���

			MOVQ		[EDI-8], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_7

		LoopX_7: // 16�N���b�N��T�C�N��
			// Step4
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM2, [ESI-8]		// 1// UnPair

			MOVD		MM3, [ESI-12]		// 2
			PUNPCKLDQ	MM0, MM2			// 1				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVD		MM5, [ESI-16]		// 2// UnPair

			MOVQ		MM1, [EDI]			// 1
			PUNPCKLDQ	MM3, MM5			// 2				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM4, [EDI+8]		// 2
			MOVQ		MM2, MM0			// 1

			MOVQ		MM5, MM3			// 2
			PAND		MM0, MM1			// 1

			PAND		MM3, MM4			// 2
			PXOR		MM2, MM1			// 1

			PXOR		MM5, MM4			// 2
			PSRLD		MM2, 1				// 1

			PSRLD		MM5, 1				// 2
			PAND		MM2, MM6			// 1

			PAND		MM5, MM6			// 2
			PADDD		MM0, MM2			// 1

			PADDD		MM3, MM5			// 2
			ADD			ESI, -16

			MOVQ		[EDI], MM0			// 1// UnPair

			MOVQ		[EDI+8], MM3	// 2// UnPair
			
			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_7				// UnPair

		EndLoop_7:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_7				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:�L	 1:1�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOVD		MM6, Mask7f
			MOV			EDX, nHeight

			MOVQ		MM0, MM6
			MOV			ECX, nWidth

			PUNPCKLDQ	MM6, MM0								// 0x007f7f7f
			MOV			EDI, lpDst

			MOV			ESI, lpSrc
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_8:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_8: // 11�N���b�N��T�C�N��
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [EDI]
			MOVQ		MM2, MM0

			PAND		MM0, MM1
			PXOR		MM2, MM1

			PSRLD		MM2, 1				// UnPair

			PAND		MM2, MM6
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PADDD		MM0, MM2
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_8									// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_8:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_8

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_8										// if ( EY >= 0 )

			SUB			EBX, EY2									// Y�̕␳
			ADD			ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

		SkipY_8:
			DEC			EDX
			JNZ			LoopY_8

			EMMS
		}
	} // if


	return 0;
} // BlendBltFastHalfM


//////////////////////////////////////
//	BlendBltM
//	�����F�L���RGB�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BlendBltM(CDIB32* lpDIB32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
							,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BlendBltM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::BlendBltM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:��	 RGB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			EDI, lpDst

			MOVQ		MM0, MM7
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM7, MM0								// ColKey
			PXOR		MM4, MM4

			MOVD		MM2, dwSrcRGBRate
			PUNPCKLBW	MM2, MM4								// WORD�P�ʂŏ�Z����̂�

			MOVD		MM3, dwDstRGBRate
			PUNPCKLBW	MM3, MM4

		LoopY_9:
			SHR			ECX, 1
			JNB			LoopX_9									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [EDI]
			MOVQ		MM5, MM0

			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�
			MOVQ		MM6, MM1

			PUNPCKLBW	MM1, MM4
			PMULLW		MM0, MM2

			PMULLW		MM1, MM3
			PCMPEQD		MM5, MM7								//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4			// UnPair

			PXOR		MM6, MM0			// UnPair			// (P ^ Dst)

			PAND		MM6, MM5			// UnPair			// & mask

			PXOR		MM0, MM6								// P ^ ()
			ADD			EDI, 4

			ADD			ESI, -4				// AGI���

			MOVD		[EDI-4], MM0

			OR			ECX, ECX
			JZ			EndLoop_9

		LoopX_9:// 22�N���b�N��T�C�N�� (PMUL�� 3�N���b�N���� 30�N���b�N)
			// Step2
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM5, [ESI-8]		// 2
			PUNPCKLBW	MM0, MM4			// 1				// WORD�P�ʂŏ�Z����̂�

			MOVD		MM1, [EDI]			// 1
			PUNPCKLBW	MM5, MM4			// 2				// WORD�P�ʂŏ�Z����̂�

			PMULLW		MM0, MM2			// 1
			PUNPCKLBW	MM1, MM4			// 1

			MOVD		MM6, [EDI+4]		// 2
			PMULLW		MM1, MM3			// 1

			PUNPCKLBW	MM6, MM4			// 2
			PMULLW		MM5, MM2			// 2

			PMULLW		MM6, MM3			// 2
			NOP			// PMUL�X�g�[�����

			PADDUSB		MM0, MM1			// 1// UnPair

			PSRLW		MM0, 8				// 1// UnPair

			PACKUSWB	MM0, MM4			// 1
			PADDUSB		MM5, MM6			// 2

			MOVD		MM6, [ESI-4]		// 1
			PSRLW		MM5, 8				// 2

			MOVD		MM1, [EDI]			// 1
			PACKUSWB	MM5, MM4			// 2

			PCMPEQD		MM6, MM7			// 1				//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0
			PXOR		MM1, MM0			// 1				// (P ^ Dst)

			PAND		MM1, MM6			// 1//UnPair		// & mask

			MOVD		MM6, [ESI-8]		// 2
			PXOR		MM0, MM1			// 1				// P ^ ()

			MOVD		MM1, [EDI+4]		// 2
			PCMPEQD		MM6, MM7			// 2				//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0

			PXOR		MM1, MM5			// 2// UnPair		// (P ^ Dst)

			PAND		MM1, MM6			// 2				// & mask
			ADD			EDI, 8

			PXOR		MM5, MM1			// 2				// P ^ ()
			ADD			ESI, -8

			MOVD		[EDI-8], MM0			// 1// UnPair

			MOVD		[EDI-4], MM5		// 2// UnPair

			DEC			ECX
			JNZ			LoopX_9

		EndLoop_9:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_9				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 RGB�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			EDI, lpDst

			MOVQ		MM0, MM7
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM7, MM0								// ColKey
			PXOR		MM4, MM4

			MOVD		MM2, dwSrcRGBRate
			PUNPCKLBW	MM2, MM4								// WORD�P�ʂŏ�Z����̂�

			MOVD		MM3, dwDstRGBRate
			PUNPCKLBW	MM3, MM4

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;

		LoopY_10:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_10: // 18�N���b�N��T�C�N�� (PMUL�� 3�N���b�N���� 22�N���b�N)
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [EDI]
			MOVQ		MM5, MM0

			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�
			MOVQ		MM6, MM1

			PUNPCKLBW	MM1, MM4
			PMULLW		MM0, MM2

			PMULLW		MM1, MM3
			PCMPEQD		MM5, MM7								//	�����F��Src�̔�r���True:MM0=0xffffffffffffffff False:MM0=0

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4			// UnPair

			PXOR		MM6, MM0			// UnPair			// (P ^ Dst)

			PAND		MM6, MM5								// & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM0, MM6								// P ^ ()
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_10								// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_10:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_10

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_10									// if ( EY >= 0 )

			SUB			EBX, EY2									// Y�̕␳
			ADD			ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

		SkipY_10:
			DEC			EDX
			JNZ			LoopY_10

			EMMS
		}
	} // if

	
	return 0;
} // BlendBltM


//////////////////////////////////////
//	BlendBltFastM
//	�����F������RGB�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BlendBltFastM(CDIB32* lpDIB32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
								,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BlendBltFastM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::BlendBltFastM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:��	 RGB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			PXOR		MM4, MM4
			MOVD		MM2, dwSrcRGBRate

			PUNPCKLBW	MM2, MM4								// WORD�P�ʂŏ�Z����̂�
			MOVD		MM3, dwDstRGBRate

			PUNPCKLBW	MM3, MM4			// UnPair

		LoopY_11:
			SHR			ECX, 1
			JNB			LoopX_11								//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [EDI]
			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�

			PMULLW		MM0, MM2
			PUNPCKLBW	MM1, MM4

			PMULLW		MM1, MM3
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4			// UnPair
			ADD			EDI, 4

			ADD			ESI, -4				// AGI���

			MOVD		[EDI-4], MM0

			OR			ECX, ECX
			JZ			EndLoop_11

		LoopX_11:// 15�N���b�N��T�C�N�� (PMUL�� 3�N���b�N���� 23�N���b�N)
			// Step2
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM5, [ESI-8]		// 2
			PUNPCKLBW	MM0, MM4			// 1				// WORD�P�ʂŏ�Z����̂�

			MOVD		MM1, [EDI]			// 1
			PUNPCKLBW	MM5, MM4			// 2				// WORD�P�ʂŏ�Z����̂�

			PMULLW		MM0, MM2			// 1
			PUNPCKLBW	MM1, MM4			// 1

			MOVD		MM6, [EDI+4]		// 2
			PMULLW		MM1, MM3			// 1

			PUNPCKLBW	MM6, MM4			// 2
			PMULLW		MM5, MM2			// 2

			PMULLW		MM6, MM3			// 2
			NOP			// PMUL�X�g�[�����

			PADDUSB		MM0, MM1			// 1// UnPair

			PSRLW		MM0, 8				// 1// UnPair

			PACKUSWB	MM0, MM4			// 1
			PADDUSB		MM5, MM6			// 2

			PSRLW		MM5, 8				// 2
			ADD			EDI, 8

			PACKUSWB	MM5, MM4			// 2
			ADD			ESI, -8

			MOVD		[EDI-8], MM0			// 1// UnPair

			MOVD		[EDI-4], MM5		// 2// UnPair

			DEC			ECX
			JNZ			LoopX_11

		EndLoop_11:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_11			// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:�L	 RGB�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			PXOR		MM4, MM4
			MOVD		MM2, dwSrcRGBRate

			PUNPCKLBW	MM2, MM4								// WORD�P�ʂŏ�Z����̂�
			MOVD		MM3, dwDstRGBRate

			PUNPCKLBW	MM3, MM4
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_12:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_12: // 14�N���b�N��T�C�N�� (PMUL�� 3�N���b�N���� 18�N���b�N)
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [EDI]
			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�

			PMULLW		MM0, MM2
			PUNPCKLBW	MM1, MM4

			PMULLW		MM1, MM3			// UnPair
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PACKUSWB	MM0, MM4
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_12								// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_12:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_12

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_12										// if ( EY >= 0 )

			SUB			EBX, EY2									// Y�̕␳
			ADD			ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

		SkipY_12:
			DEC			EDX
			JNZ			LoopY_12

			EMMS
		}
	} // if

	
	return 0;
} // BlendBltFastM


//////////////////////////////////////
//	AddColorBltM
//	�����F�L���ADD�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::AddColorBltM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::AddColorBltM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::AddColorBltM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:��	 ADD�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			EDI, lpDst

			MOVQ		MM0, MM7
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM7, MM0			// UnPair			// ColKey

		LoopY_13:
			SHR			ECX, 1
			JNB			Skip_13									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [EDI]
			MOVQ		MM1, MM0

			PADDUSB		MM0, MM2
			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000

			PXOR		MM2, MM0			// UnPair			// (P ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// P ^ ()
			ADD			EDI, 4

			ADD			ESI, -4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_13

		Skip_13:
			SHR			ECX, 1
			JNB			LoopX_13								//	4�̔{�����ۂ��̔���
			// Step	2
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM3, [ESI-8]		// UnPair

			MOVQ		MM2, [EDI]
			PUNPCKLDQ	MM0, MM3								// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM1, MM0
			PADDUSB		MM0, MM2

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (P ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// P ^ ()
			ADD			EDI, 8

			ADD			ESI, -8				// AGI���

			MOVQ		[EDI-8], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_13

		LoopX_13:// 16�N���b�N��T�C�N��
			// Step4
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM6, [ESI-8]		// 1// UnPair

			MOVD		MM3, [ESI-12]		// 2
			PUNPCKLDQ	MM0, MM6			// 1				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVD		MM6, [ESI-16]		// 2
			MOVQ		MM1, MM0			// 1

			MOVQ		MM2, [EDI]			// 1
			PUNPCKLDQ	MM3, MM6			// 2				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM5, [EDI+8]		// 2
			MOVQ		MM4, MM3			// 2

			PADDUSB		MM0, MM2			// 1
			PADDUSB		MM3, MM5			// 2

			PCMPEQD		MM1, MM7			// 1				// ColKey:0xffffffff NonColKey:0x00000000
			PCMPEQD		MM4, MM7			// 2				// ColKey:0xffffffff NonColKey:0x00000000

			PXOR		MM2, MM0			// 1				// (P ^ dst)
			PXOR		MM5, MM3			// 2				// (P ^ dst)

			PAND		MM2, MM1			// 1				// & mask
			PAND		MM5, MM4			// 2				// & mask

			PXOR		MM0, MM2			// 1				// P ^ ()
			PXOR		MM3, MM5			// 2				// P ^ ()

			ADD			ESI, -16			// AGI���

			MOVQ		[EDI], MM0			// 1// UnPair

			MOVQ		[EDI+8], MM3		// 2// UnPair

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_13			// UnPair

		EndLoop_13:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_13			// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 ADD�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			EDI, lpDst

			MOVQ		MM0, MM7
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM7, MM0								// ColKey
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_14:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_14: // 11�N���b�N��T�C�N��
			MOVD		MM0, [ESI-4]			// UnPair

			MOVD		MM2, [EDI]
			MOVQ		MM1, MM0

			PADDUSB		MM0, MM2
			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000

			PXOR		MM2, MM0			// UnPair			// (P ^ dst)

			PAND		MM2, MM1								// & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM0, MM2								// P ^ ()
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_14								// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_14:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_14

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_14										// if ( EY >= 0 )

			SUB			EBX, EY2									// Y�̕␳
			ADD			ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

		SkipY_14:
			DEC			EDX
			JNZ			LoopY_14

			EMMS
		}
	} // if


	return 0;
} // AddColorBltM


//////////////////////////////////////
//	AddColorBltFastM
//	�����F������ADD�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::AddColorBltFastM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::AddColorBltFastM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::AddColorBltFastM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:��	 ADD�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

		LoopY_15:
			SHR			ECX, 1
			JNB			Skip_15									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [EDI]			// UnPair

			PADDUSB		MM0, MM2
			ADD			EDI, 4

			ADD			ESI, -4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_15

		Skip_15:
			SHR			ECX, 1
			JNB			LoopX_15									//	4�̔{�����ۂ��̔���
			// Step	2
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM3, [ESI-8]		// UnPair

			MOVQ		MM2, [EDI]
			PUNPCKLDQ	MM0, MM3								// �\�[�X�� 2�s�N�Z�����~���[����

			PADDUSB		MM0, MM2
			ADD			EDI, 8

			ADD			ESI, -8				// AGI���

			MOVQ		[EDI-8], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_15

		LoopX_15:// 11�N���b�N��T�C�N��
			// Step4
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM6, [ESI-8]		// 1// UnPair

			MOVD		MM3, [ESI-12]		// 2
			PUNPCKLDQ	MM0, MM6			// 1				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVD		MM6, [ESI-16]		// 2// UnPair

			MOVQ		MM2, [EDI]		// 1
			PUNPCKLDQ	MM3, MM6			// 2				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM5, [EDI+8]		// 2
			PADDUSB		MM0, MM2			// 1

			PADDUSB		MM3, MM5			// 2
			ADD			EDI, 16

			ADD			ESI, -16			// AGI���

			MOVQ		[EDI-16], MM0		// 1// UnPair

			MOVQ		[EDI-8], MM3		// 2// UnPair

			DEC			ECX
			JNZ			LoopX_15

		EndLoop_15:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_15			// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:�L	 ADD�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;
		
		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;

		LoopY_16:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_16: // 8�N���b�N��T�C�N��
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [EDI]			// UnPair

			PADDUSB		MM0, MM2
			ADD			ESI, AddSrcPixel						// �������̉��Z

			ADD			EDI, 4
			ADD			EAX, EX									// EX += 2*DX;
			
			JNB			SkipX_16			// UnPair			// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_16:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_16

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_16										// if ( EY >= 0 )

			SUB			EBX, EY2									// Y�̕␳
			ADD			ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

		SkipY_16:
			DEC			EDX
			JNZ			LoopY_16

			EMMS
		}
	} // if


	return 0;
} // AddColorBltFastM


//////////////////////////////////////
//	SubColorBltM
//	�����F�L���SUB�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::SubColorBltM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::SubColorBltM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::SubColorBltM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:��	 SUB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			EDI, lpDst

			MOVQ		MM0, MM7
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM7, MM0			// UnPair			// ColKey

		LoopY_17:
			SHR			ECX, 1
			JNB			Skip_17									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [EDI]
			MOVQ		MM1, MM0

			MOVQ		MM3, MM2
			PSUBUSB		MM2, MM0

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM3, MM2								// (P ^ dst)

			PAND		MM3, MM1			// UnPair			// & mask

			PXOR		MM2, MM3								// P ^ ()
			ADD			EDI, 4

			ADD			ESI, -4				// AGI���

			MOVD		[EDI-4], MM2		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_17

		Skip_17:
			SHR			ECX, 1
			JNB			LoopX_17								//	4�̔{�����ۂ��̔���
			// Step	2
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [ESI-8]		// UnPair

			MOVQ		MM2, [EDI]
			PUNPCKLDQ	MM0, MM1								// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM1, MM0
			MOVQ		MM3, MM2

			PSUBUSB		MM2, MM0
			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000

			PXOR		MM3, MM2			// UnPair			// (P ^ dst)

			PAND		MM3, MM1			// UnPair			// & mask

			PXOR		MM2, MM3								// P ^ ()
			ADD			EDI, 8

			ADD			ESI, -8				// AGI���

			MOVQ		[EDI-8], MM2		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_17

		LoopX_17:// 17�N���b�N��T�C�N��
			// Step4
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM1, [ESI-8]		// 1// UnPair

			MOVD		MM3, [ESI-12]		// 2
			PUNPCKLDQ	MM0, MM1			// 1				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVD		MM4, [ESI-16]		// 2
			MOVQ		MM1, MM0			// 1

			MOVQ		MM2, [EDI]			// 1
			PUNPCKLDQ	MM3, MM4			// 2				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM5, [EDI+8]		// 2
			MOVQ		MM4, MM3			// 2

			MOVQ		MM6, MM2			// 1
			PSUBUSB		MM2, MM0			// 1

			PCMPEQD		MM1, MM7			// 1				// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM6, MM2			// 1				// (P ^ dst)

			PAND		MM6, MM1			// 1				// & mask
			PSUBUSB		MM5, MM3			// 2

			PXOR		MM2, MM6			// 1				// P ^ ()
			PCMPEQD		MM4, MM7			// 2				// ColKey:0xffffffff NonColKey:0x00000000

			MOVQ		MM6, [EDI+8]		// 2// UnPair

			PXOR		MM6, MM5			// 2// UnPair		// (P ^ dst)

			PAND		MM6, MM4			// 2				// & mask
			ADD			EDI, 16

			PXOR		MM5, MM6			// 2				// P ^ ()
			ADD			ESI, -16			// AGI���

			MOVQ		[EDI-16], MM2		// 1// UnPair

			MOVQ		[EDI-8], MM5		// 2// UnPair

			DEC			ECX
			JNZ			LoopX_17

		EndLoop_17:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_17			// UnPair
	
			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 ADD�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			EDI, lpDst

			MOVQ		MM0, MM7
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM7, MM0								// ColKey
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_18:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_18: // 11�N���b�N��T�C�N��
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [EDI]
			MOVQ		MM1, MM0

			MOVQ		MM3, MM2
			PSUBUSB		MM2, MM0

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM3, MM2								// (P ^ dst)

			PAND		MM3, MM1								// & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM2, MM3								// P ^ ()
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_18								// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_18:
			MOVD		[EDI-4], MM2		// UnPair

			DEC			ECX
			JNZ			LoopX_18

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_18										// if ( EY >= 0 )

			SUB			EBX, EY2									// Y�̕␳
			ADD			ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

		SkipY_18:
			DEC			EDX
			JNZ			LoopY_18

			EMMS
		}
	} // if


	return 0;
} // SubColorBltM


//////////////////////////////////////
//	SubColorBltFastM
//	�����F������SUB�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32PMMX::SubColorBltFastM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::SubColorBltFastM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32PMMX::SubColorBltFastM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:��	 SUB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

		LoopY_19:
			SHR			ECX, 1
			JNB			Skip_19								//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [EDI]			// UnPair

			PSUBUSB		MM2, MM0
			ADD			EDI, 4

			ADD			ESI, -4				// AGI���

			MOVD		[EDI-4], MM2		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_19

		Skip_19:
			SHR			ECX, 1
			JNB			LoopX_19								//	4�̔{�����ۂ��̔���
			// Step	2
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [ESI-8]		// UnPair

			MOVQ		MM2, [EDI]
			PUNPCKLDQ	MM0, MM1								// �\�[�X�� 2�s�N�Z�����~���[����

			PSUBUSB		MM2, MM0
			ADD			EDI, 8

			ADD			ESI, -8				// AGI���

			MOVQ		[EDI-8], MM2		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_19

		LoopX_19:// 11�N���b�N��T�C�N��
			// Step4
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM1, [ESI-8]		// 1// UnPair

			MOVD		MM3, [ESI-12]		// 2
			PUNPCKLDQ	MM0, MM1			// 1				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVD		MM4, [ESI-16]		// 2// UnPair

			MOVQ		MM2, [EDI]			// 1
			PUNPCKLDQ	MM3, MM4			// 2				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM5, [EDI+8]		// 2
			PSUBUSB		MM2, MM0			// 1

			PSUBUSB		MM5, MM3			// 1
			ADD			ESI, -16			// AGI���

			MOVQ		[EDI], MM2			// 1// UnPair

			MOVQ		[EDI+8], MM5		// 2// UnPair

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_19			// UnPair

		EndLoop_19:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_19			// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:�L	 ADD�u�����h:�L	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;

		LoopY_20:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_20: // 8�N���b�N��T�C�N��
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [EDI]			// UnPair

			PSUBUSB		MM2, MM0
			ADD			ESI, AddSrcPixel						// �������̉��Z

			ADD			EDI, 4
			ADD			EAX, EX									// EX += 2*DX;

			JNB			SkipX_20			// UnPair			// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_20:
			MOVD		[EDI-4], MM2		// UnPair

			DEC			ECX
			JNZ			LoopX_20

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_20										// if ( EY >= 0 )

			SUB			EBX, EY2									// Y�̕␳
			ADD			ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

		SkipY_20:
			DEC			EDX
			JNZ			LoopY_20

			EMMS
		}
	} // if



	return 0;
} // SubColorBltFastM


////////////////////////////////////////////////////////////////////
//	���̃v���[���ɑ΂���G�t�F�N�g
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////
//	AddColorFast
//	�����F������ADD�u�����h�]�� (���v���[��)
//////////////////////////////////////////////////
LRESULT CDIB32PMMX::AddColorFast(DWORD dwAddRGB,LPRECT lpSrcRect)
{
// �N���b�s���O���̏���
	//	�\�[�X��`��NULL�Ȃ�΁A�S��
	if (lpSrcRect==NULL)
	{
		m_rcSrcRect = m_rcRect;
	}
	else
	{
		m_rcSrcRect = *lpSrcRect;
		if ( m_rcSrcRect.left > m_rcSrcRect.right )
		{
			return 1;	// invalid rect
		}
	}
	//	�N���b�v�̈�
	LPRECT lpClip;
	lpClip = &m_rcRect;

		// �N���b�s���O����
		// this yaneurao clipping algorithm is changed by tia
		int t;
		t = lpClip->left  - m_rcSrcRect.left;
		if ( t > 0 ) { m_rcSrcRect.left = lpClip->left; }
		t = lpClip->top	  - m_rcSrcRect.top;
		if ( t > 0 ) { m_rcSrcRect.top = lpClip->top; }
		t = m_rcSrcRect.right - lpClip->right;
		if ( t > 0 ) { m_rcSrcRect.right = lpClip->right; }
		t = m_rcSrcRect.bottom - lpClip->bottom;
		if ( t > 0 ) { m_rcSrcRect.bottom = lpClip->bottom; }

		//	invalid rect ?
		if (m_rcSrcRect.left >= m_rcSrcRect.right ||
			m_rcSrcRect.top	 >= m_rcSrcRect.bottom) return 1;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcSrcRect.right - m_rcSrcRect.left;
	int		nHeight = m_rcSrcRect.bottom - m_rcSrcRect.top;
	DWORD	nAddSrcWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
	DWORD*	lpSrc = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcSrcRect.left<<2) + m_rcSrcRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:��	 ADD�u�����h:�L	  --------------
		_asm
		{
			MOVD		MM2, dwAddRGB
			MOV			EDX, nHeight

			MOVQ		MM0, MM2
			MOV			ECX, nWidth

			PUNPCKLDQ	MM2, MM0
			MOV			ESI, lpSrc

		LoopY_21:
			SHR			ECX, 1
			JNB			Skip_21									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI]			// UnPair

			PADDUSB		MM0, MM2
			NOP			// AGI���

			NOP								// UnPair

			MOVD		[ESI], MM0			// UnPair

			ADD			ESI, 4				// UnPair

			OR			ECX, ECX
			JZ			EndLoop_21

		Skip_21:
			SHR			ECX, 1
			JNB			LoopX_21								//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			PADDUSB		MM0, MM2
			NOP			// AGI���

			NOP								// UnPair

			MOVQ		[ESI], MM0			// UnPair

			ADD			ESI, 8				// UnPair

			OR			ECX, ECX
			JZ			EndLoop_21

		LoopX_21: // 7�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// 1// UnPair

			MOVQ		MM3, [ESI+8]		// 2
			PADDUSB		MM0, MM2			// 1

			PADDUSB		MM3, MM2			// 2// AGI���
			
			MOVQ		[ESI], MM0			// UnPair

			MOVQ		[ESI+8], MM3		// UnPair

			ADD			ESI, 16
			DEC			ECX

			JNZ			LoopX_21			// UnPair

		EndLoop_21:
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�
			MOV			ECX, nWidth								//	ECX�̍X�V

			DEC			EDX
			JNZ			LoopY_21

			EMMS
		}


	return 0;
} // AddColorFast


//////////////////////////////////////////////////
//	SubColorFast
//	�����F������Sub�u�����h�]�� (���v���[��)
//////////////////////////////////////////////////
LRESULT CDIB32PMMX::SubColorFast(DWORD dwSubRGB,LPRECT lpSrcRect)
{
// �N���b�s���O���̏���
	//	�\�[�X��`��NULL�Ȃ�΁A�S��
	if (lpSrcRect==NULL)
	{
		m_rcSrcRect = m_rcRect;
	}
	else
	{
		m_rcSrcRect = *lpSrcRect;
		if ( m_rcSrcRect.left > m_rcSrcRect.right )
		{
			return 1;	// invalid rect
		}
	}
	//	�N���b�v�̈�
	LPRECT lpClip;
	lpClip = &m_rcRect;

		// �N���b�s���O����
		// this yaneurao clipping algorithm is changed by tia
		int t;
		t = lpClip->left  - m_rcSrcRect.left;
		if ( t > 0 ) { m_rcSrcRect.left = lpClip->left; }
		t = lpClip->top	  - m_rcSrcRect.top;
		if ( t > 0 ) { m_rcSrcRect.top = lpClip->top; }
		t = m_rcSrcRect.right - lpClip->right;
		if ( t > 0 ) { m_rcSrcRect.right = lpClip->right; }
		t = m_rcSrcRect.bottom - lpClip->bottom;
		if ( t > 0 ) { m_rcSrcRect.bottom = lpClip->bottom; }

		//	invalid rect ?
		if (m_rcSrcRect.left >= m_rcSrcRect.right ||
			m_rcSrcRect.top	 >= m_rcSrcRect.bottom) return 1;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcSrcRect.right - m_rcSrcRect.left;
	int		nHeight = m_rcSrcRect.bottom - m_rcSrcRect.top;
	DWORD	nSubSrcWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
	DWORD*	lpSrc = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcSrcRect.left<<2) + m_rcSrcRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:��	 Sub�u�����h:�L	  --------------
		_asm
		{
			MOVD		MM2, dwSubRGB
			MOV			EDX, nHeight

			MOVQ		MM0, MM2
			MOV			ECX, nWidth

			PUNPCKLDQ	MM2, MM0
			MOV			ESI, lpSrc

		LoopY_21:
			SHR			ECX, 1
			JNB			Skip_21									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI]			// UnPair

			PSUBUSB		MM0, MM2
			NOP			// AGI���

			NOP								// UnPair

			MOVD		[ESI], MM0			// UnPair

			ADD			ESI, 4				// UnPair

			OR			ECX, ECX
			JZ			EndLoop_21

		Skip_21:
			SHR			ECX, 1
			JNB			LoopX_21								//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			PSUBUSB		MM0, MM2
			NOP			// AGI���

			NOP								// UnPair

			MOVQ		[ESI], MM0			// UnPair

			ADD			ESI, 8				// UnPair

			OR			ECX, ECX
			JZ			EndLoop_21

		LoopX_21: // 7�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// 1// UnPair

			MOVQ		MM3, [ESI+8]		// 2
			PSUBUSB		MM0, MM2			// 1

			PSUBUSB		MM3, MM2			// 2// AGI���
			
			MOVQ		[ESI], MM0			// UnPair

			MOVQ		[ESI+8], MM3		// UnPair

			ADD			ESI, 16
			DEC			ECX

			JNZ			LoopX_21			// UnPair

		EndLoop_21:
			ADD			ESI, nSubSrcWidth						// �N���b�v�����̈敪���΂�
			MOV			ECX, nWidth								//	ECX�̍X�V

			DEC			EDX
			JNZ			LoopY_21

			EMMS
		}


	return 0;
} // SubColorFast

//////////////////////////////////////
//	�l��������
//////////////////////////////////////
//	�X�L�����y�яo�͗p�̊J�n�_�i�r���C�r���j�Ƃt�u�X�e�b�v�ʁi�o���C�o���j�\����
//			�A�Z���u���ł̏ꍇ�����ɍŏ�U,V�̏I�_���W�����b���ꑤ�ŋ��߂�
//			�����Ă����B�����Ċ���Z����Ċi�[
struct StartAndStep	{
	int		nStartX;
	int		nStartY;
	int		nStepUX;
	int		nStepUY;
	int		nStepVX;
	int		nStepVY;
};
//	�S�_�̃|�C���g�A�N�Z�X�p�񋓑�
enum Point { A, B, C, D, POINT_NUM };
//	�o�͐�̃\�[�g�p�\����
struct	DstSort {
	Point	p;
	int		x;
	int		y;
};
static int pointsort(const void* element1, const void* element2)
{
	int		iRes;
	iRes = (static_cast<const DstSort*> (element1))->y - (static_cast<const DstSort*> (element2))->y;
	if (iRes == 0) {
		return (static_cast<const DstSort*> (element1))->x - (static_cast<const DstSort*> (element2))->x;
	} else {
		return iRes;
	}
}

LRESULT CDIB32P5::MorphBlt(CDIB32* lpDIB32, LPRECT lpSrcRect, LPPOINT lpDstPoint, LPRECT lpClip)
{
	if (lpSrcRect==NULL) {
		lpSrcRect = lpDIB32->GetRect();
	}

	//	�q�d�b�s�|���o�n�h�m�s�ϊ�
	POINT	lpSrcPoint[4];
	lpSrcPoint[0].x = lpSrcRect->left;
	lpSrcPoint[0].y = lpSrcRect->top;
	lpSrcPoint[1].x = lpSrcRect->right - 1;
	lpSrcPoint[1].y = lpSrcRect->top;
	lpSrcPoint[2].x = lpSrcRect->left;
	lpSrcPoint[2].y = lpSrcRect->bottom - 1;
	lpSrcPoint[3].x = lpSrcRect->right - 1;
	lpSrcPoint[3].y = lpSrcRect->bottom - 1;
	return CDIB32P5::MorphBlt(lpDIB32, lpSrcPoint, lpDstPoint, lpClip, false);
}

LRESULT CDIB32P5::MorphBlt(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPRECT lpDstRect, LPRECT lpClip)
{
	if (lpDstRect==NULL){
		lpDstRect = GetRect();
	}

	//	�q�d�b�s�|���o�n�h�m�s�ϊ�
	POINT	lpDstPoint[4];
	lpDstPoint[0].x = lpDstRect->left;
	lpDstPoint[0].y = lpDstRect->top;
	lpDstPoint[1].x = lpDstRect->right - 1;
	lpDstPoint[1].y = lpDstRect->top;
	lpDstPoint[2].x = lpDstRect->left;
	lpDstPoint[2].y = lpDstRect->bottom - 1;
	lpDstPoint[3].x = lpDstRect->right - 1;
	lpDstPoint[3].y = lpDstRect->bottom - 1;
	return CDIB32P5::MorphBlt(lpDIB32, lpSrcPoint, lpDstPoint, lpClip, true);
}

LRESULT CDIB32P5::MorphBlt(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPPOINT lpDstPoint, LPRECT lpClip, bool bContinual)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32P5::MorphBlt��m_lpdwSrc == NULL");
	{
	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32P5::MorphBlt��p->GetPtr() == NULL");
	}

	//	�N���b�v�̈�ϊ�
	if (lpClip == NULL) {
		lpClip = GetRect();
	}
	//	�X�L�����E�o�͗p�J�n�_�Ƃt�u�X�e�b�v�o�b�t�@
	StartAndStep lpScanSS[3], lpPutSS[3];
	//	�������e�����ɂ�����X�L������(Sx,Sy)-(Ex,Ey)�����邽�߂̏o�͂x����
	int	lpnScanU[3], lpnScanV[3];
	//	�X�L��������S�_�̒��_�A���o�b�t�@
	static const Point	lpPointConnect[POINT_NUM][3] = {
		//	 A			  B			   C			D
		{ B, C, D }, { A, D, C }, { A, D, B }, { B, C, A }
	};
	//	�o�͎l�p�`�̒��_�x�����\�[�g���ʊi�[�o�b�t�@
	DstSort	lpDstSort[POINT_NUM];
	for (int i = A; i < POINT_NUM; i++) {
		lpDstSort[i].p = static_cast<Point> (i);
		lpDstSort[i].x = (lpDstPoint + i)->x;
		lpDstSort[i].y = (lpDstPoint + i)->y;
	}
	//	�o�̓|�C���g���x�������ɂ���B
	qsort(lpDstSort, POINT_NUM, sizeof(lpDstSort[0]), pointsort);
	//	�o�͂��邌���������������J�E���^�o�b�t�@
	int			lpPutCount[3];
	lpPutCount[0] = lpDstSort[1].y - lpDstSort[0].y;
	lpPutCount[1] = lpDstSort[2].y - lpDstSort[1].y;
	lpPutCount[2] = lpDstSort[3].y - lpDstSort[2].y;
	
	//	���_�o�̓f�[�^�ǂݏo���ăZ�b�g
	lpPutSS[0].nStartX = lpDstSort[0].x;
	lpPutSS[0].nStartY = lpDstSort[0].y;
	//	���_�����UV�����߂�
	Point	point0 = lpDstSort[0].p;
	Point	point1 = lpPointConnect[lpDstSort[0].p][0];
	Point	point2 = lpPointConnect[lpDstSort[0].p][1];
	int nVect = (((lpDstPoint + point2)->x - (lpDstPoint + point0)->x) *
				  ((lpDstPoint + point1)->y - (lpDstPoint + point0)->y)) -
				(((lpDstPoint + point2)->y - (lpDstPoint + point0)->y) *
				 ((lpDstPoint + point1)->x - (lpDstPoint + point0)->x));
	Point	pointU, pointV, pointExc;
	if (0 > nVect) {
		pointV = lpPointConnect[lpDstSort[0].p][0];
		pointU = lpPointConnect[lpDstSort[0].p][1];
	} else {
		pointU = lpPointConnect[lpDstSort[0].p][0];
		pointV = lpPointConnect[lpDstSort[0].p][1];
	}
	pointExc = lpPointConnect[lpDstSort[0].p][2];
	//	�ŏ��̒��_����̂������l���߂ăZ�b�g
	lpnScanU[0] = (lpDstPoint + pointU)->y - lpDstSort[0].y;
	lpnScanV[0] = (lpDstPoint + pointV)->y - lpDstSort[0].y;
	lpPutSS[0].nStepUX = lpDstSort[0].x - (lpDstPoint + pointU)->x;
	lpPutSS[0].nStepVX = lpDstSort[0].x - (lpDstPoint + pointV)->x;
	lpScanSS[0].nStartX = (lpSrcPoint + lpDstSort[0].p)->x;
	lpScanSS[0].nStartY = (lpSrcPoint + lpDstSort[0].p)->y;
	lpScanSS[0].nStepUX = lpScanSS[0].nStartX - (lpSrcPoint + pointU)->x;
	lpScanSS[0].nStepUY = lpScanSS[0].nStartY - (lpSrcPoint + pointU)->y;
	lpScanSS[0].nStepVX = lpScanSS[0].nStartX - (lpSrcPoint + pointV)->x;
	lpScanSS[0].nStepVY = lpScanSS[0].nStartY - (lpSrcPoint + pointV)->y;
	if ((lpDstSort[1].p != pointU) && (lpDstSort[1].p != pointV))
		return 0;
	//	�������l���߂ăZ�b�g
	if (lpDstSort[1].p == pointU) {
		lpnScanU[1] = (lpDstPoint + pointExc)->y - lpDstSort[1].y;
		lpnScanV[1] = 0;
		lpPutSS[1].nStepUX = lpDstSort[1].x - (lpDstPoint + pointExc)->x;
		lpPutSS[1].nStepVX = 0;
		lpScanSS[1].nStartX = (lpSrcPoint + pointU)->x;
		lpScanSS[1].nStartY = (lpSrcPoint + pointU)->y;
		lpScanSS[1].nStepUX = lpScanSS[1].nStartX - (lpSrcPoint + pointExc)->x;
		lpScanSS[1].nStepUY = lpScanSS[1].nStartY - (lpSrcPoint + pointExc)->y;
		lpScanSS[1].nStepVX = 0;
		lpScanSS[1].nStepVY = 0;
		if (lpDstSort[2].p == pointV) {
			lpnScanU[2] = 0;
			lpnScanV[2] = (lpDstPoint + pointExc)->y - lpDstSort[2].y;
			lpPutSS[2].nStepUX = 0;
			lpPutSS[2].nStepVX = lpDstSort[2].x - (lpDstPoint + pointExc)->x;
			lpScanSS[2].nStartX = (lpSrcPoint + pointV)->x;
			lpScanSS[2].nStartY = (lpSrcPoint + pointV)->y;
			lpScanSS[2].nStepUX = 0;
			lpScanSS[2].nStepUY = 0;
			lpScanSS[2].nStepVX = lpScanSS[2].nStartX - (lpSrcPoint + pointExc)->x;
			lpScanSS[2].nStepVY = lpScanSS[2].nStartY - (lpSrcPoint + pointExc)->y;
		} else {
			lpnScanV[2] = 0;
			lpnScanU[2] = (lpDstPoint + pointV)->y - lpDstSort[2].y;
			lpPutSS[2].nStepVX = 0;
			lpPutSS[2].nStepUX = lpDstSort[2].x - (lpDstPoint + pointV)->x;
			lpScanSS[2].nStartX = (lpSrcPoint + pointExc)->x;
			lpScanSS[2].nStartY = (lpSrcPoint + pointExc)->y;
			lpScanSS[2].nStepVX = 0;
			lpScanSS[2].nStepVY = 0;
			lpScanSS[2].nStepUX = lpScanSS[2].nStartX - (lpSrcPoint + pointV)->x;
			lpScanSS[2].nStepUY = lpScanSS[2].nStartY - (lpSrcPoint + pointV)->y;
		}
	} else {
		lpnScanU[1] = 0;
		lpnScanV[1] = (lpDstPoint + pointExc)->y - lpDstSort[1].y;
		lpPutSS[1].nStepUX = 0;
		lpPutSS[1].nStepVX = lpDstSort[1].x - (lpDstPoint + pointExc)->x;
		lpScanSS[1].nStartX = (lpSrcPoint + pointV)->x;
		lpScanSS[1].nStartY = (lpSrcPoint + pointV)->y;
		lpScanSS[1].nStepUX = 0;
		lpScanSS[1].nStepUY = 0;
		lpScanSS[1].nStepVX = lpScanSS[1].nStartX - (lpSrcPoint + pointExc)->x;
		lpScanSS[1].nStepVY = lpScanSS[1].nStartY - (lpSrcPoint + pointExc)->y;
		if (lpDstSort[2].p == pointU) {
			lpnScanV[2] = 0;
			lpnScanU[2] = (lpDstPoint + pointExc)->y - lpDstSort[2].y;
			lpPutSS[2].nStepVX = 0;
			lpPutSS[2].nStepUX = lpDstSort[2].x - (lpDstPoint + pointExc)->x;
			lpScanSS[2].nStartX = (lpSrcPoint + pointU)->x;
			lpScanSS[2].nStartY = (lpSrcPoint + pointU)->y;
			lpScanSS[2].nStepVX = 0;
			lpScanSS[2].nStepVY = 0;
			lpScanSS[2].nStepUX = lpScanSS[2].nStartX - (lpSrcPoint + pointExc)->x;
			lpScanSS[2].nStepUY = lpScanSS[2].nStartY - (lpSrcPoint + pointExc)->y;
		} else {
			lpnScanU[2] = 0;
			lpnScanV[2] = (lpDstPoint + pointU)->y - lpDstSort[2].y;
			lpPutSS[2].nStepUX = 0;
			lpPutSS[2].nStepVX = lpDstSort[2].x - (lpDstPoint + pointU)->x;
			lpScanSS[2].nStartX = (lpSrcPoint + pointExc)->x;
			lpScanSS[2].nStartY = (lpSrcPoint + pointExc)->y;
			lpScanSS[2].nStepUX = 0;
			lpScanSS[2].nStepUY = 0;
			lpScanSS[2].nStepVX = lpScanSS[2].nStartX - (lpSrcPoint + pointU)->x;
			lpScanSS[2].nStepVY = lpScanSS[2].nStartY - (lpSrcPoint + pointU)->y;
		}
	}
	//	�X�L�������C�����́i�r���C�r���j�|�i�d���C�d���j�̈ʒu��
	//	�X�L�������C���̕ω��X�e�b�v
	int		nScanStepSX, nScanStepSY, nScanStepEX, nScanStepEY;
	int		nScanLineSX, nScanLineSY, nScanLineEX, nScanLineEY;
	int		nScanLineStepX, nScanLineStepY;
	//	�o�̓��C���́i�r���C�x���j�|�i�d���C�x���j
	int		nPutStepSX, nPutStepEX;
	int		nPutLineSX, nPutLineEX;
	int		nPutLineLeng;
	//	�o�̓o�b�t�@�x�[�X�A�h���X
	CDIB32Base*	p = lpDIB32->GetDIB32BasePtr();
	DWORD*		lpDstBase = (DWORD*)((BYTE*)m_lpdwSrc + (lpPutSS[0].nStartY * m_lPitch) - m_lPitch );
	DWORD*		lpSrcBase = p->m_lpdwSrc;
	DWORD*		lpSrcEnd =	(DWORD*)((BYTE*)(p->m_lpdwSrc) + p->m_lPitch * m_rcRect.bottom);
	DWORD		lSrcPitch = p->m_lPitch;
	DWORD		lDstPitch = m_lPitch;
	DWORD**		lplpLineStart = p->m_lpdwLineStart;
	DWORD		ColKey	= p->m_dwColorKey;
	DWORD		SrcClip = 0;
	//	�X�L�����E�o�͂̃N���b�s���O�̈�
	DWORD*		lpDstStart = (DWORD*)((BYTE*)m_lpdwSrc + (lpClip->top * m_lPitch));
	DWORD*		lpDstEnd = (DWORD*)((BYTE*)m_lpdwSrc + (lpClip->bottom * m_lPitch));
	DWORD*		lpDstLineStart = lpDstBase + lpClip->left;
	DWORD*		lpDstLineEnd = lpDstBase + lpClip->right;
	int			nSrcSX = p->m_rcRect.left;
	int			nSrcSY = p->m_rcRect.top;
	int			nSrcEX = p->m_rcRect.right;
	int			nSrcEY = p->m_rcRect.bottom;
	DWORD		nPutLineAdj = (bContinual ? 0 : 1 << 20);
	//	�A�Z���u���ł̃o�b�N�A�b�v���[�N
	DWORD		nScanPointX, nScanPointY;
	//	����������
		nScanStepSX = lpScanSS[0].nStepUX; //  / lpnScanU[0]
		nScanStepSY = lpScanSS[0].nStepUY; //  / lpnScanU[0]
		nScanStepEX = lpScanSS[0].nStepVX; //  / lpnScanV[0]
		nScanStepEY = lpScanSS[0].nStepVY; //  / lpnScanV[0]
		nScanLineSX = lpScanSS[0].nStartX;
		nScanLineSY = lpScanSS[0].nStartY;
		nScanLineEX = lpScanSS[0].nStartX;
		nScanLineEY = lpScanSS[0].nStartY;
		nPutStepSX = lpPutSS[0].nStepUX;	// / lpnScanU[0]
		nPutStepEX = lpPutSS[0].nStepVX;	// / lpnScanV[0]
		if (lpPutCount[0]) {
			nPutLineSX = lpPutSS[0].nStartX;
			nPutLineEX = lpPutSS[0].nStartX;
		} else {
			nPutLineSX = lpPutSS[0].nStartX;
			nPutLineEX = lpPutSS[0].nStartX - lpPutSS[0].nStepVX;
		}
		_asm {
			sal		nPutLineSX,20
			mov		edi,lpDstBase

			sal		nPutLineEX,20
			mov		esi,lpSrcBase
			////////////////////////////////
			or		lpnScanU[0*4],0
			jz		conv_skip5
			
			//	�f�[�^�̌Œ菬���ϊ�
			mov		eax, nScanStepSX
			mov		edx, eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[0*4]
			mov		nScanStepSX, eax
			///////////////////////////////
			mov		eax, nScanStepSY
			mov		edx, eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[0*4]
			mov		nScanStepSY, eax
			//////////////////////////////
			sal		nScanLineSX,20
			sal		nScanLineSY,20
			/////////////////////////////
			mov		eax,nPutStepSX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[0*4]
			mov		nPutStepSX,eax
conv_skip5:
			or		lpnScanV[0*4],0
			jz		conv_skip6
			///////////////////////////////
			mov		eax,nScanStepEX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[0*4]
			mov		nScanStepEX, eax
			///////////////////////////////
			mov		eax,nScanStepEY
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[0*4]
			mov		nScanStepEY,eax
			//////////////////////////////
			sal		nScanLineEX,20
			sal		nScanLineEY,20
			////////////////////////////
			mov		eax,nPutStepEX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[0*4]
			mov		nPutStepEX,eax
conv_skip6:
			or		lpPutCount[0*4],0
			jz		end_ly1
			//	���������[�v
loop_ly1:
			//	�X�L�����J�n�I���ʒu�X�V
			mov		eax,nScanStepSX
			mov		ebx,nScanLineSX

			mov		edx,nScanStepSY
			sub		ebx,eax

			mov		ecx,nScanLineSY
			mov		nScanLineSX,ebx

			sub		ecx,edx
			mov		eax,nScanStepEX

			mov		nScanLineSY,ecx
			mov		ebx,nScanLineEX
			
			sub		ebx,eax
			mov		ecx,nScanLineEY
			
			mov		edx,nScanStepEY
			sub		ecx,edx
			
			mov		nScanLineEX,ebx
			mov		nScanLineEY,ecx
			
			//	�o�̓x�[�X���C���X�V���o�̓��C���X�V
			add		edi,lDstPitch
			mov		ecx,lDstPitch

			mov		lpDstBase,edi
			add		lpDstLineStart,ecx

			mov		eax,nPutStepSX
			mov		ebx,nPutLineSX

			sub		ebx,eax				//	EDI + (EBX >> 20) << 4���������݊J�n�ʒu
			add		lpDstLineEnd,ecx

			mov		edx,nPutStepEX
			mov		ecx,nPutLineEX

			mov		nPutLineSX,ebx
			sub		ecx,edx

			and		ebx,0xfff00000
			mov		nPutLineEX,ecx

			and		ecx,0xfff00000

			sub		ecx,ebx				//	ECX�ɃX�L�������C����(12:20)

			mov		nPutLineLeng, ecx
//			jc		line_skip1				// �L�����[���ƁA��-���̌v�Z�ɁA�L�����[���g�p����̂ŁA
			js		line_skip1				// ���������Ĕ�Ԃ悤�ɂɕύX���� '00.10.3.

			sar		ecx,20

			jz		line_skip1

			cmp		edi,lpDstStart		//	�o�̓��C���͈̓`�F�b�N
			jc		line_skip1

			cmp		edi,lpDstEnd
			jnc		line_skip1

			sar		ebx,20-2

			add		edi,ebx				//	edi���|�o�͊J�n�A�h���X
			//	�X�L�������C���ړ��䗦�����߂�
			cmp		ecx,1
			jbe		dclip_start1

			mov		eax, nPutLineAdj

			sub		nPutLineLeng,eax
			mov		eax,nScanLineSX

			mov		ebx,nScanLineEX

			sub		eax,ebx

			mov		edx,eax

			sal		eax,20
	
			sar		edx,12

			idiv	nPutLineLeng

			mov		nScanLineStepX,eax
			
			mov		eax,nScanLineSY
			mov		ebx,nScanLineEY

			sub		eax,ebx

			mov		edx,eax

			sal		eax,20

			sar		edx,12

			idiv	nPutLineLeng
			mov		nScanLineStepY,eax
dclip_start1:										//	�o�͂̉E�N���b�s���O
			lea		ebx,[edi+ecx*4]					//	bx <- �o�͏I���A�h���X
			sub		ebx,lpDstLineEnd
			jbe		dst_clip1
			sar		ebx,2
			sub		ecx,ebx
			jbe		line_skip1
dst_clip1:
			//	�X�L�����J�n�ʒu���o��	
			mov		ebx,nScanLineSX
			mov		eax,nScanLineSY
dst_clip2:											//	�o�͂̍��N���b�s���O
			cmp		edi,lpDstLineStart
			jnc		line_loop1

			sub		ebx,nScanLineStepX
			sub		eax,nScanLineStepY

			add		edi,4
			dec		ecx
			jz		line_skip1
			jmp		dst_clip2
line_loop1:
			// this loop changed by tia
			mov		nScanPointX,ebx
			mov		nScanPointY,eax

			sar		eax,20
			mov		esi,lplpLineStart

			sar		ebx,20

			cmp		ebx,nSrcSX
			jc		line_skip2

			cmp		eax,nSrcSY
			jc		line_skip2
			
			cmp		ebx,nSrcEX
			jnc		line_skip2
			
			cmp		eax,nSrcEY
			jnc		line_skip2
			
			mov		esi,[esi+4*eax]
			mov		eax,nScanPointY

			mov		edx,[esi+4*ebx]
			mov		ebx,nScanPointX

			cmp		edx,ColKey
			jz		ckey_skip1

			mov		[edi],edx
ckey_skip1:	sub		ebx,nScanLineStepX

			sub		eax,nScanLineStepY
			add		edi,4

			dec		ecx
			jnz		line_loop1

			jmp		line_skip1
line_skip2:	mov		eax,nScanPointY
			mov		ebx,nScanPointX
			//	�X�L�����ʒu�ړ�
			sub		ebx,nScanLineStepX
			sub		eax,nScanLineStepY
			//	�o�͈ʒu�ړ�
			add		edi,4
			dec		ecx
			jnz		line_loop1
line_skip1:
			mov		edi,lpDstBase
			//	���C���J�E���^�f�N�������g
			dec		lpPutCount[0]
			jnz		loop_ly1
end_ly1:
		}
		//	����������
		if (lpnScanU[1]) {
			nScanStepSX = lpScanSS[1].nStepUX; //  / lpnScanU[1]
			nScanStepSY = lpScanSS[1].nStepUY; //  / lpnScanU[1]
			nScanLineSX = lpScanSS[1].nStartX;
			nScanLineSY = lpScanSS[1].nStartY;
			nPutStepSX = lpPutSS[1].nStepUX;	// / lpnScanU[1]
		}
		if (lpnScanV[1]) {
			nScanStepEX = lpScanSS[1].nStepVX; //  / lpnScanV[1]
			nScanStepEY = lpScanSS[1].nStepVY; //  / lpnScanV[1]
			nScanLineEX = lpScanSS[1].nStartX;
			nScanLineEY = lpScanSS[1].nStartY;
			nPutStepEX = lpPutSS[1].nStepVX;	// / lpnScanV[1]
		}
		_asm {
			or		lpnScanU[1*4],0
			jz		conv_skip1
			//	�f�[�^�̌Œ菬���ϊ�
			mov		eax, nScanStepSX
			mov		edx, eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[1*4]
			mov		nScanStepSX, eax
			///////////////////////////////
			mov		eax, nScanStepSY
			mov		edx, eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[1*4]
			mov		nScanStepSY, eax
			//////////////////////////////
			sal		nScanLineSX,20
			sal		nScanLineSY,20
			/////////////////////////////
			mov		eax,nPutStepSX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[1*4]
			mov		nPutStepSX,eax
conv_skip1:
			or		lpnScanV[1*4],0
			jz		conv_skip2
			///////////////////////////////
			mov		eax,nScanStepEX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[1*4]
			mov		nScanStepEX, eax
			///////////////////////////////
			mov		eax,nScanStepEY
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[1*4]
			mov		nScanStepEY,eax
			//////////////////////////////
			sal		nScanLineEX,20
			sal		nScanLineEY,20
			////////////////////////////
			mov		eax,nPutStepEX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[1*4]
			mov		nPutStepEX,eax
conv_skip2:
			or		lpPutCount[1*4],0
			jz		end_ly2
			//	���������[�v
loop_ly2:
			//	�X�L�����J�n�I���ʒu�X�V
			mov		eax,nScanStepSX
			mov		ebx,nScanLineSX

			mov		edx,nScanStepSY
			sub		ebx,eax

			mov		ecx,nScanLineSY
			mov		nScanLineSX,ebx

			sub		ecx,edx
			mov		eax,nScanStepEX
			
			mov		nScanLineSY,ecx
			mov		ebx,nScanLineEX
			
			sub		ebx,eax
			mov		ecx,nScanLineEY
			
			mov		edx,nScanStepEY
			sub		ecx,edx
			
			mov		nScanLineEX,ebx
			mov		nScanLineEY,ecx
			
			//	�o�̓x�[�X���C���X�V���o�̓��C���X�V
			add		edi,lDstPitch
			mov		ecx,lDstPitch

			mov		lpDstBase,edi
			add		lpDstLineStart,ecx

			mov		eax,nPutStepSX
			mov		ebx,nPutLineSX

			sub		ebx,eax				//	EDI + (EBX >> 20) << 4���������݊J�n�ʒu
			add		lpDstLineEnd,ecx

			mov		edx,nPutStepEX
			mov		ecx,nPutLineEX

			mov		nPutLineSX,ebx
			sub		ecx,edx

			and		ebx,0xfff00000
			mov		nPutLineEX,ecx

			and		ecx,0xfff00000

			sub		ecx,ebx				//	ECX�ɃX�L�������C����(12:20)

			mov		nPutLineLeng, ecx
//			jc		line_skip3
			js		line_skip3				// ���������Ĕ�Ԃ悤�ɂɕύX���� '00.10.3.

			sar		ecx,20

			jz		line_skip3

			cmp		edi,lpDstStart		//	�o�̓��C���͈̓`�F�b�N
			jc		line_skip3

			cmp		edi,lpDstEnd
			jnc		line_skip3

			sar		ebx,20-2

			add		edi,ebx				//	edi���|�o�͊J�n�A�h���X
			//	�X�L�������C���ړ��䗦�����߂�
			cmp		ecx,1
			jbe		dclip_start2

			mov		eax, nPutLineAdj

			sub		nPutLineLeng,eax
			mov		eax,nScanLineSX

			mov		ebx,nScanLineEX

			sub		eax,ebx

			mov		edx,eax

			sal		eax,20
	
			sar		edx,12

			idiv	nPutLineLeng

			mov		nScanLineStepX,eax
			
			mov		eax,nScanLineSY
			mov		ebx,nScanLineEY

			sub		eax,ebx

			mov		edx,eax

			sal		eax,20

			sar		edx,12

			idiv	nPutLineLeng
			mov		nScanLineStepY,eax
dclip_start2:										//	�o�͂̉E�N���b�s���O
			lea		ebx,[edi+ecx*4]					//	bx <- �o�͏I���A�h���X
			sub		ebx,lpDstLineEnd
			jbe		dst_clip3
			sar		ebx,2
			sub		ecx,ebx
			jbe		line_skip3
dst_clip3:
			//	�X�L�����J�n�ʒu���o��	
			mov		ebx,nScanLineSX
			mov		eax,nScanLineSY
dst_clip4:											//	�o�͂̍��N���b�s���O
			cmp		edi,lpDstLineStart
			jnc		line_loop2

			sub		ebx,nScanLineStepX
			sub		eax,nScanLineStepY

			add		edi,4
			dec		ecx
			jz		line_skip3
			jmp		dst_clip4
line_loop2:
			mov		nScanPointX,ebx
			mov		nScanPointY,eax

			sar		eax,20
			mov		esi,lplpLineStart

			sar		ebx,20

			cmp		ebx,nSrcSX
			jc		line_skip4

			cmp		eax,nSrcSY
			jc		line_skip4
			
			cmp		ebx,nSrcEX
			jnc		line_skip4
			
			cmp		eax,nSrcEY
			jnc		line_skip4
			
			mov		esi,[esi+4*eax]
			mov		eax,nScanPointY

			mov		edx,[esi+4*ebx]
			mov		ebx,nScanPointX

			cmp		edx,ColKey
			jz		ckey_skip2

			mov		[edi],edx
ckey_skip2:	sub		ebx,nScanLineStepX

			sub		eax,nScanLineStepY
			add		edi,4

			dec		ecx
			jnz		line_loop2

			jmp		line_skip3
line_skip4:	mov		eax,nScanPointY
			mov		ebx,nScanPointX
			//	�X�L�����ʒu�ړ�
			sub		ebx,nScanLineStepX
			sub		eax,nScanLineStepY
			//	�o�͈ʒu�ړ�
			add		edi,4
			dec		ecx
			jnz		line_loop2
line_skip3:
			mov		edi,lpDstBase
			//	���C���J�E���^�f�N�������g
			dec		lpPutCount[1 * 4]
			jnz		loop_ly2
end_ly2:
		}
		//	����������
	if (lpPutCount[2]) {
		if (lpnScanU[2]) {
			nScanStepSX = lpScanSS[2].nStepUX; //  / lpnScanU[2]
			nScanStepSY = lpScanSS[2].nStepUY; //  / lpnScanU[2]
			nScanLineSX = lpScanSS[2].nStartX;
			nScanLineSY = lpScanSS[2].nStartY;
			nPutStepSX = lpPutSS[2].nStepUX;	// / lpnScanU[2]
		}
		if (lpnScanV[2]) {
			nScanStepEX = lpScanSS[2].nStepVX; //  / lpnScanV[2]
			nScanStepEY = lpScanSS[2].nStepVY; //  / lpnScanV[2]
			nScanLineEX = lpScanSS[2].nStartX;
			nScanLineEY = lpScanSS[2].nStartY;
			nPutStepEX = lpPutSS[2].nStepVX;	// / lpnScanV[2]
		}
		_asm {
			or		lpnScanU[2*4],0
			jz		conv_skip3
			//	�f�[�^�̌Œ菬���ϊ�
			mov		eax, nScanStepSX
			mov		edx, eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[2*4]
			mov		nScanStepSX, eax
			///////////////////////////////
			mov		eax, nScanStepSY
			mov		edx, eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[2*4]
			mov		nScanStepSY, eax
			//////////////////////////////
			sal		nScanLineSX,20
			sal		nScanLineSY,20
			/////////////////////////////
			mov		eax,nPutStepSX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[2*4]
			mov		nPutStepSX,eax
conv_skip3:
			or		lpnScanV[2*4],0
			jz		conv_skip4
			///////////////////////////////
			mov		eax,nScanStepEX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[2*4]
			mov		nScanStepEX, eax
			///////////////////////////////
			mov		eax,nScanStepEY
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[2*4]
			mov		nScanStepEY,eax
			//////////////////////////////
			sal		nScanLineEX,20
			sal		nScanLineEY,20
			////////////////////////////
			mov		eax,nPutStepEX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[2*4]
			mov		nPutStepEX,eax
conv_skip4:
			//	���������[�v
loop_ly3:
			//	�X�L�����J�n�I���ʒu�X�V
			mov		eax,nScanStepSX
			mov		ebx,nScanLineSX

			mov		edx,nScanStepSY
			sub		ebx,eax

			mov		ecx,nScanLineSY
			mov		nScanLineSX,ebx

			sub		ecx,edx
			mov		eax,nScanStepEX
			
			mov		nScanLineSY,ecx
			mov		ebx,nScanLineEX
			
			sub		ebx,eax
			mov		ecx,nScanLineEY
			
			mov		edx,nScanStepEY
			sub		ecx,edx
			
			mov		nScanLineEX,ebx
			mov		nScanLineEY,ecx
			
			//	�o�̓x�[�X���C���X�V���o�̓��C���X�V
			add		edi,lDstPitch
			mov		ecx,lDstPitch

			mov		lpDstBase,edi
			add		lpDstLineStart,ecx

			mov		eax,nPutStepSX
			mov		ebx,nPutLineSX

			sub		ebx,eax				//	EDI + (EBX >> 20) << 4���������݊J�n�ʒu
			add		lpDstLineEnd,ecx

			mov		edx,nPutStepEX
			mov		ecx,nPutLineEX

			mov		nPutLineSX,ebx
			sub		ecx,edx

			and		ebx,0xfff00000
			mov		nPutLineEX,ecx

			and		ecx,0xfff00000

			sub		ecx,ebx				//	ECX�ɃX�L�������C����(12:20)

			mov		nPutLineLeng, ecx
//			jc		line_skip5
			js		line_skip5				// ���������Ĕ�Ԃ悤�ɂɕύX���� '00.10.3.

			sar		ecx,20

			jz		line_skip5

			cmp		edi,lpDstStart		//	�o�̓��C���͈̓`�F�b�N
			jc		line_skip5

			cmp		edi,lpDstEnd
			jnc		line_skip5

			sar		ebx,20-2

			add		edi,ebx				//	edi���|�o�͊J�n�A�h���X
			//	�X�L�������C���ړ��䗦�����߂�
			cmp		ecx,1
			jbe		dclip_start3

			mov		eax, nPutLineAdj

			sub		nPutLineLeng,eax
			mov		eax,nScanLineSX

			mov		ebx,nScanLineEX

			sub		eax,ebx

			mov		edx,eax

			sal		eax,20
	
			sar		edx,12

			idiv	nPutLineLeng

			mov		nScanLineStepX,eax
			
			mov		eax,nScanLineSY
			mov		ebx,nScanLineEY

			sub		eax,ebx

			mov		edx,eax

			sal		eax,20

			sar		edx,12

			idiv	nPutLineLeng
			mov		nScanLineStepY,eax
dclip_start3:										//	�o�͂̉E�N���b�s���O
			lea		ebx,[edi+ecx*4]					//	bx <- �o�͏I���A�h���X
			sub		ebx,lpDstLineEnd
			jbe		dst_clip5
			sar		ebx,2
			sub		ecx,ebx
			jbe		line_skip5
dst_clip5:
			//	�X�L�����J�n�ʒu���o��	
			mov		ebx,nScanLineSX
			mov		eax,nScanLineSY
dst_clip6:											//	�o�͂̍��N���b�s���O
			cmp		edi,lpDstLineStart
			jnc		line_loop3

			sub		ebx,nScanLineStepX
			sub		eax,nScanLineStepY

			add		edi,4
			dec		ecx
			jz		line_skip5
			jmp		dst_clip6
line_loop3:
			mov		nScanPointX,ebx
			mov		nScanPointY,eax

			sar		eax,20
			mov		esi,lplpLineStart

			sar		ebx,20

			cmp		ebx,nSrcSX
			jc		line_skip6

			cmp		eax,nSrcSY
			jc		line_skip6
			
			cmp		ebx,nSrcEX
			jnc		line_skip6
			
			cmp		eax,nSrcEY
			jnc		line_skip6
			
			mov		esi,[esi+4*eax]
			mov		eax,nScanPointY

			mov		edx,[esi+4*ebx]
			mov		ebx,nScanPointX
			
			cmp		edx,ColKey
			jz		ckey_skip3

			mov		[edi],edx
ckey_skip3:	sub		ebx,nScanLineStepX

			sub		eax,nScanLineStepY
			add		edi,4

			dec		ecx
			jnz		line_loop3

			jmp		line_skip5
line_skip6:	mov		eax,nScanPointY
			mov		ebx,nScanPointX
			//	�X�L�����ʒu�ړ�
			sub		ebx,nScanLineStepX
			sub		eax,nScanLineStepY
			//	�o�͈ʒu�ړ�
			add		edi,4
			dec		ecx
			jnz		line_loop3
line_skip5:
			mov		edi,lpDstBase
			//	���C���J�E���^�f�N�������g
			dec		lpPutCount[2 * 4]
			jnz		loop_ly3
		}
	}
	return 0;
}


LRESULT CDIB32P5::MorphBltFast(CDIB32* lpDIB32, LPRECT lpSrcRect, LPPOINT lpDstPoint, LPRECT lpClip)
{
	if (lpSrcRect==NULL) {
		lpSrcRect = lpDIB32->GetRect();
	}
	//	�q�d�b�s�|���o�n�h�m�s�ϊ�
	POINT	lpSrcPoint[4];
	lpSrcPoint[0].x = lpSrcRect->left;
	lpSrcPoint[0].y = lpSrcRect->top;
	lpSrcPoint[1].x = lpSrcRect->right;
	lpSrcPoint[1].y = lpSrcRect->top;
	lpSrcPoint[2].x = lpSrcRect->left;
	lpSrcPoint[2].y = lpSrcRect->bottom;
	lpSrcPoint[3].x = lpSrcRect->right;
	lpSrcPoint[3].y = lpSrcRect->bottom;
	return CDIB32P5::MorphBltFast(lpDIB32, lpSrcPoint, lpDstPoint, lpClip, false);
}

LRESULT CDIB32P5::MorphBltFast(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPRECT lpDstRect, LPRECT lpClip)
{
	//	�q�d�b�s�|���o�n�h�m�s�ϊ�
	POINT	lpDstPoint[4];
	lpDstPoint[0].x = lpDstRect->left;
	lpDstPoint[0].y = lpDstRect->top;
	lpDstPoint[1].x = lpDstRect->right;
	lpDstPoint[1].y = lpDstRect->top;
	lpDstPoint[2].x = lpDstRect->left;
	lpDstPoint[2].y = lpDstRect->bottom;
	lpDstPoint[3].x = lpDstRect->right;
	lpDstPoint[3].y = lpDstRect->bottom;
	return CDIB32P5::MorphBltFast(lpDIB32, lpSrcPoint, lpDstPoint, lpClip, true);
}

LRESULT CDIB32P5::MorphBltFast(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPPOINT lpDstPoint, LPRECT lpClip, bool bContinual)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32P5::MorphBltFast��m_lpdwSrc == NULL");
	{
	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32P5::MorphBltFast��p->GetPtr() == NULL");
	}

	//	�N���b�v�̈�ϊ�
	if (lpClip == NULL) {
		lpClip = &m_rcRect;
	}
	//	�X�L�����E�o�͗p�J�n�_�Ƃt�u�X�e�b�v�o�b�t�@
	StartAndStep lpScanSS[3], lpPutSS[3];
	//	�������e�����ɂ�����X�L������(Sx,Sy)-(Ex,Ey)�����邽�߂̏o�͂x����
	int	lpnScanU[3], lpnScanV[3];
	//	�X�L��������S�_�̒��_�A���o�b�t�@
	static const Point	lpPointConnect[POINT_NUM][3] = {
		//	 A			  B			   C			D
		{ B, C, D }, { A, D, C }, { A, D, B }, { B, C, A }
	};
	//	�o�͎l�p�`�̒��_�x�����\�[�g���ʊi�[�o�b�t�@
	DstSort	lpDstSort[POINT_NUM];
	for (int i = A; i < POINT_NUM; i++) {
		lpDstSort[i].p = static_cast<Point> (i);
		lpDstSort[i].x = (lpDstPoint + i)->x;
		lpDstSort[i].y = (lpDstPoint + i)->y;
	}
	//	�o�̓|�C���g���x�������ɂ���B
	qsort(lpDstSort, POINT_NUM, sizeof(lpDstSort[0]), pointsort);
	//	�o�͂��邌���������������J�E���^�o�b�t�@
	int			lpPutCount[3];
	lpPutCount[0] = lpDstSort[1].y - lpDstSort[0].y;
	lpPutCount[1] = lpDstSort[2].y - lpDstSort[1].y;
	lpPutCount[2] = lpDstSort[3].y - lpDstSort[2].y;
	
	//	���_�o�̓f�[�^�ǂݏo���ăZ�b�g
	lpPutSS[0].nStartX = lpDstSort[0].x;
	lpPutSS[0].nStartY = lpDstSort[0].y;
	//	���_�����UV�����߂�
	Point	point0 = lpDstSort[0].p;
	Point	point1 = lpPointConnect[lpDstSort[0].p][0];
	Point	point2 = lpPointConnect[lpDstSort[0].p][1];
	int nVect = (((lpDstPoint + point2)->x - (lpDstPoint + point0)->x) *
				  ((lpDstPoint + point1)->y - (lpDstPoint + point0)->y)) -
				(((lpDstPoint + point2)->y - (lpDstPoint + point0)->y) *
				 ((lpDstPoint + point1)->x - (lpDstPoint + point0)->x));
	Point	pointU, pointV, pointExc;
	if (0 > nVect) {
		pointV = lpPointConnect[lpDstSort[0].p][0];
		pointU = lpPointConnect[lpDstSort[0].p][1];
	} else {
		pointU = lpPointConnect[lpDstSort[0].p][0];
		pointV = lpPointConnect[lpDstSort[0].p][1];
	}
	pointExc = lpPointConnect[lpDstSort[0].p][2];
	//	�ŏ��̒��_����̂������l���߂ăZ�b�g
	lpnScanU[0] = (lpDstPoint + pointU)->y - lpDstSort[0].y;
	lpnScanV[0] = (lpDstPoint + pointV)->y - lpDstSort[0].y;
	lpPutSS[0].nStepUX = lpDstSort[0].x - (lpDstPoint + pointU)->x;
	lpPutSS[0].nStepVX = lpDstSort[0].x - (lpDstPoint + pointV)->x;
	lpScanSS[0].nStartX = (lpSrcPoint + lpDstSort[0].p)->x;
	lpScanSS[0].nStartY = (lpSrcPoint + lpDstSort[0].p)->y;
	lpScanSS[0].nStepUX = lpScanSS[0].nStartX - (lpSrcPoint + pointU)->x;
	lpScanSS[0].nStepUY = lpScanSS[0].nStartY - (lpSrcPoint + pointU)->y;
	lpScanSS[0].nStepVX = lpScanSS[0].nStartX - (lpSrcPoint + pointV)->x;
	lpScanSS[0].nStepVY = lpScanSS[0].nStartY - (lpSrcPoint + pointV)->y;
	if ((lpDstSort[1].p != pointU) && (lpDstSort[1].p != pointV))
		return 0;
	//	�������l���߂ăZ�b�g
	if (lpDstSort[1].p == pointU) {
		lpnScanU[1] = (lpDstPoint + pointExc)->y - lpDstSort[1].y;
		lpnScanV[1] = 0;
		lpPutSS[1].nStepUX = lpDstSort[1].x - (lpDstPoint + pointExc)->x;
		lpPutSS[1].nStepVX = 0;
		lpScanSS[1].nStartX = (lpSrcPoint + pointU)->x;
		lpScanSS[1].nStartY = (lpSrcPoint + pointU)->y;
		lpScanSS[1].nStepUX = lpScanSS[1].nStartX - (lpSrcPoint + pointExc)->x;
		lpScanSS[1].nStepUY = lpScanSS[1].nStartY - (lpSrcPoint + pointExc)->y;
		lpScanSS[1].nStepVX = 0;
		lpScanSS[1].nStepVY = 0;
		if (lpDstSort[2].p == pointV) {
			lpnScanU[2] = 0;
			lpnScanV[2] = (lpDstPoint + pointExc)->y - lpDstSort[2].y;
			lpPutSS[2].nStepUX = 0;
			lpPutSS[2].nStepVX = lpDstSort[2].x - (lpDstPoint + pointExc)->x;
			lpScanSS[2].nStartX = (lpSrcPoint + pointV)->x;
			lpScanSS[2].nStartY = (lpSrcPoint + pointV)->y;
			lpScanSS[2].nStepUX = 0;
			lpScanSS[2].nStepUY = 0;
			lpScanSS[2].nStepVX = lpScanSS[2].nStartX - (lpSrcPoint + pointExc)->x;
			lpScanSS[2].nStepVY = lpScanSS[2].nStartY - (lpSrcPoint + pointExc)->y;
		} else {
			lpnScanV[2] = 0;
			lpnScanU[2] = (lpDstPoint + pointV)->y - lpDstSort[2].y;
			lpPutSS[2].nStepVX = 0;
			lpPutSS[2].nStepUX = lpDstSort[2].x - (lpDstPoint + pointV)->x;
			lpScanSS[2].nStartX = (lpSrcPoint + pointExc)->x;
			lpScanSS[2].nStartY = (lpSrcPoint + pointExc)->y;
			lpScanSS[2].nStepVX = 0;
			lpScanSS[2].nStepVY = 0;
			lpScanSS[2].nStepUX = lpScanSS[2].nStartX - (lpSrcPoint + pointV)->x;
			lpScanSS[2].nStepUY = lpScanSS[2].nStartY - (lpSrcPoint + pointV)->y;
		}
	} else {
		lpnScanU[1] = 0;
		lpnScanV[1] = (lpDstPoint + pointExc)->y - lpDstSort[1].y;
		lpPutSS[1].nStepUX = 0;
		lpPutSS[1].nStepVX = lpDstSort[1].x - (lpDstPoint + pointExc)->x;
		lpScanSS[1].nStartX = (lpSrcPoint + pointV)->x;
		lpScanSS[1].nStartY = (lpSrcPoint + pointV)->y;
		lpScanSS[1].nStepUX = 0;
		lpScanSS[1].nStepUY = 0;
		lpScanSS[1].nStepVX = lpScanSS[1].nStartX - (lpSrcPoint + pointExc)->x;
		lpScanSS[1].nStepVY = lpScanSS[1].nStartY - (lpSrcPoint + pointExc)->y;
		if (lpDstSort[2].p == pointU) {
			lpnScanV[2] = 0;
			lpnScanU[2] = (lpDstPoint + pointExc)->y - lpDstSort[2].y;
			lpPutSS[2].nStepVX = 0;
			lpPutSS[2].nStepUX = lpDstSort[2].x - (lpDstPoint + pointExc)->x;
			lpScanSS[2].nStartX = (lpSrcPoint + pointU)->x;
			lpScanSS[2].nStartY = (lpSrcPoint + pointU)->y;
			lpScanSS[2].nStepVX = 0;
			lpScanSS[2].nStepVY = 0;
			lpScanSS[2].nStepUX = lpScanSS[2].nStartX - (lpSrcPoint + pointExc)->x;
			lpScanSS[2].nStepUY = lpScanSS[2].nStartY - (lpSrcPoint + pointExc)->y;
		} else {
			lpnScanU[2] = 0;
			lpnScanV[2] = (lpDstPoint + pointU)->y - lpDstSort[2].y;
			lpPutSS[2].nStepUX = 0;
			lpPutSS[2].nStepVX = lpDstSort[2].x - (lpDstPoint + pointU)->x;
			lpScanSS[2].nStartX = (lpSrcPoint + pointExc)->x;
			lpScanSS[2].nStartY = (lpSrcPoint + pointExc)->y;
			lpScanSS[2].nStepUX = 0;
			lpScanSS[2].nStepUY = 0;
			lpScanSS[2].nStepVX = lpScanSS[2].nStartX - (lpSrcPoint + pointU)->x;
			lpScanSS[2].nStepVY = lpScanSS[2].nStartY - (lpSrcPoint + pointU)->y;
		}
	}
	//	�X�L�������C�����́i�r���C�r���j�|�i�d���C�d���j�̈ʒu��
	//	�X�L�������C���̕ω��X�e�b�v
	int		nScanStepSX, nScanStepSY, nScanStepEX, nScanStepEY;
	int		nScanLineSX, nScanLineSY, nScanLineEX, nScanLineEY;
	int		nScanLineStepX, nScanLineStepY;
	//	�o�̓��C���́i�r���C�x���j�|�i�d���C�x���j
	int		nPutStepSX, nPutStepEX;
	int		nPutLineSX, nPutLineEX;
	int		nPutLineLeng;
	//	�o�̓o�b�t�@�x�[�X�A�h���X
	CDIB32Base*	p = lpDIB32->GetDIB32BasePtr();
	DWORD*		lpDstBase = (DWORD*)((BYTE*)m_lpdwSrc + (lpPutSS[0].nStartY * m_lPitch) - m_lPitch );	
	DWORD*		lpSrcBase = p->m_lpdwSrc;
	DWORD		lSrcPitch = p->m_lPitch;
	DWORD		lDstPitch = m_lPitch;
	DWORD**		lplpLineStart = p->m_lpdwLineStart;
	DWORD*		lpSrcEnd =	(DWORD*)((BYTE*)(p->m_lpdwSrc) + p->m_lPitch * m_rcRect.bottom);	
	//	�X�L�����E�o�͂̃N���b�s���O�̈�
	DWORD*		lpDstStart = (DWORD*)((BYTE*)m_lpdwSrc + (lpClip->top * m_lPitch));		
	DWORD*		lpDstEnd = (DWORD*)((BYTE*)m_lpdwSrc + (lpClip->bottom * m_lPitch));	
	DWORD*		lpDstLineStart = lpDstBase + lpClip->left;
	DWORD*		lpDstLineEnd = lpDstBase + lpClip->right;
	int			nSrcSX = p->m_rcRect.left;
	int			nSrcSY = p->m_rcRect.top;
	int			nSrcEX = p->m_rcRect.right;
	int			nSrcEY = p->m_rcRect.bottom;
	DWORD		nPutLineAdj = (bContinual ? 0 : 1 << 20);
	DWORD		SrcClip = 0;
	//	�A�Z���u���ł̃o�b�N�A�b�v���[�N
	DWORD		nScanPointX, nScanPointY;
	//	����������
		nScanStepSX = lpScanSS[0].nStepUX; //  / lpnScanU[0]
		nScanStepSY = lpScanSS[0].nStepUY; //  / lpnScanU[0]
		nScanStepEX = lpScanSS[0].nStepVX; //  / lpnScanV[0]
		nScanStepEY = lpScanSS[0].nStepVY; //  / lpnScanV[0]
		nScanLineSX = lpScanSS[0].nStartX;
		nScanLineSY = lpScanSS[0].nStartY;
		nScanLineEX = lpScanSS[0].nStartX;
		nScanLineEY = lpScanSS[0].nStartY;
		nPutStepSX = lpPutSS[0].nStepUX;	// / lpnScanU[0]
		nPutStepEX = lpPutSS[0].nStepVX;	// / lpnScanV[0]
		if (lpPutCount[0]) {
			nPutLineSX = lpPutSS[0].nStartX;
			nPutLineEX = lpPutSS[0].nStartX;
		} else {
			nPutLineSX = lpPutSS[0].nStartX;
			nPutLineEX = lpPutSS[0].nStartX - lpPutSS[0].nStepVX;
		}
		_asm {
			sal		nPutLineSX,20
			mov		edi,lpDstBase

			sal		nPutLineEX,20
			mov		esi,lpSrcBase
			////////////////////////////////
			or		lpnScanU[0*4],0
			jz		conv_skip5
			
			//	�f�[�^�̌Œ菬���ϊ�
			mov		eax, nScanStepSX
			mov		edx, eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[0*4]
			mov		nScanStepSX, eax
			///////////////////////////////
			mov		eax, nScanStepSY
			mov		edx, eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[0*4]
			mov		nScanStepSY, eax
			//////////////////////////////
			sal		nScanLineSX,20
			sal		nScanLineSY,20
			/////////////////////////////
			mov		eax,nPutStepSX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[0*4]
			mov		nPutStepSX,eax
conv_skip5:
			or		lpnScanV[0*4],0
			jz		conv_skip6
			///////////////////////////////
			mov		eax,nScanStepEX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[0*4]
			mov		nScanStepEX, eax
			///////////////////////////////
			mov		eax,nScanStepEY
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[0*4]
			mov		nScanStepEY,eax
			//////////////////////////////
			sal		nScanLineEX,20
			sal		nScanLineEY,20
			////////////////////////////
			mov		eax,nPutStepEX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[0*4]
			mov		nPutStepEX,eax
conv_skip6:
			or		lpPutCount[0*4],0
			jz		end_ly1
			//	���������[�v
loop_ly1:
			//	�X�L�����J�n�I���ʒu�X�V
			mov		eax,nScanStepSX
			mov		ebx,nScanLineSX

			mov		edx,nScanStepSY
			sub		ebx,eax

			mov		ecx,nScanLineSY
			mov		nScanLineSX,ebx

			sub		ecx,edx
			mov		eax,nScanStepEX
			
			mov		nScanLineSY,ecx
			mov		ebx,nScanLineEX
			
			sub		ebx,eax
			mov		ecx,nScanLineEY
			
			mov		edx,nScanStepEY
			sub		ecx,edx
			
			mov		nScanLineEX,ebx
			mov		nScanLineEY,ecx
			
			//	�o�̓x�[�X���C���X�V���o�̓��C���X�V
			add		edi,lDstPitch
			mov		ecx,lDstPitch

			mov		lpDstBase,edi
			add		lpDstLineStart,ecx

			add		lpDstLineEnd,ecx

			mov		eax,nPutStepSX
			mov		ebx,nPutLineSX
			sub		ebx,eax				//	EDI + (EBX >> 20) << 4���������݊J�n�ʒu
			mov		nPutLineSX,ebx
			and		ebx,0xfff00000

			mov		edx,nPutStepEX
			mov		ecx,nPutLineEX
			sub		ecx,edx
			mov		nPutLineEX,ecx
			and		ecx,0xfff00000

			sub		ecx,ebx				//	ECX�ɃX�L�������C����(12:20)

			mov		nPutLineLeng, ecx
//			jc		line_skip1				// �L�����[���ƁA��-���̌v�Z�ɁA�L�����[���g�p����̂ŁA
			js		line_skip1				// ���������Ĕ�Ԃ悤�ɂɕύX���� '00.10.3.

			sar		ecx,20

			jz		line_skip1

			cmp		edi,lpDstStart		//	�o�̓��C���͈̓`�F�b�N
			jc		line_skip1

			cmp		edi,lpDstEnd
			jnc		line_skip1

			sar		ebx,20-2

			add		edi,ebx				//	edi���|�o�͊J�n�A�h���X
			//	�X�L�������C���ړ��䗦�����߂�
			cmp		ecx,1
			jbe		dclip_start1

			mov		eax, nPutLineAdj

			sub		nPutLineLeng,eax
			mov		eax,nScanLineSX

			mov		ebx,nScanLineEX

			sub		eax,ebx

			mov		edx,eax

			sal		eax,20
	
			sar		edx,12

			idiv	nPutLineLeng

			mov		nScanLineStepX,eax
			
			mov		eax,nScanLineSY
			mov		ebx,nScanLineEY

			sub		eax,ebx

			mov		edx,eax

			sal		eax,20

			sar		edx,12

			idiv	nPutLineLeng
			mov		nScanLineStepY,eax
dclip_start1:										//	�o�͂̉E�N���b�s���O
			lea		ebx,[edi+ecx*4]					//	bx <- �o�͏I���A�h���X
			sub		ebx,lpDstLineEnd
			jbe		dst_clip1
			sar		ebx,2
			sub		ecx,ebx
			jbe		line_skip1
dst_clip1:
			//	�X�L�����J�n�ʒu���o��	
			mov		ebx,nScanLineSX
			mov		eax,nScanLineSY
dst_clip2:											//	�o�͂̍��N���b�s���O
			cmp		edi,lpDstLineStart
			jnc		line_loop1

			sub		ebx,nScanLineStepX
			sub		eax,nScanLineStepY

			add		edi,4
			dec		ecx
			jz		line_skip1
			jmp		dst_clip2
line_loop1:
			// this loop changed by tia
			mov		nScanPointX,ebx
			mov		nScanPointY,eax

			sar		eax,20
			mov		esi,lplpLineStart

			sar		ebx,20

			cmp		ebx,nSrcSX
			jc		ckey_skip1

			cmp		eax,nSrcSY
			jc		ckey_skip1
			
			cmp		ebx,nSrcEX
			jnc		ckey_skip1
			
			cmp		eax,nSrcEY
			jnc		ckey_skip1

			mov		esi,[esi+4*eax]
			mov		eax,nScanPointY

			mov		edx,[esi+4*ebx]
			mov		ebx,nScanPointX

			mov		[edi],edx
			sub		ebx,nScanLineStepX

			sub		eax,nScanLineStepY
			add		edi,4

			dec		ecx
			jnz		line_loop1

			jmp		line_skip1
ckey_skip1:	mov		eax,nScanPointY
			mov		ebx,nScanPointX
			//	�X�L�����ʒu�ړ�
			sub		ebx,nScanLineStepX
			sub		eax,nScanLineStepY
			
			add		edi,4

			dec		ecx
			jnz		line_loop1
			jmp		line_skip1
/*	�g���ĂȂ�by Tia
line_skip2:	
			mov		edx,eax
			mov		esi,lplpLineStart

			sar		edx,20

			mov		esi,[esi+4*edx]
			mov		edx,ebx

			sar		edx,20

			mov		edx,[esi+4*edx]

			mov		[edi],edx
			add		edi,4

			dec		ecx
			jnz		line_skip2
*/
line_skip1:
			mov		edi,lpDstBase
			//	���C���J�E���^�f�N�������g
			dec		lpPutCount[0]
			jnz		loop_ly1
end_ly1:
		}


		//	����������
		if (lpnScanU[1]) {
			nScanStepSX = lpScanSS[1].nStepUX; //  / lpnScanU[1]
			nScanStepSY = lpScanSS[1].nStepUY; //  / lpnScanU[1]
			nScanLineSX = lpScanSS[1].nStartX;
			nScanLineSY = lpScanSS[1].nStartY;
			nPutStepSX = lpPutSS[1].nStepUX;	// / lpnScanU[1]
		}
		if (lpnScanV[1]) {
			nScanStepEX = lpScanSS[1].nStepVX; //  / lpnScanV[1]
			nScanStepEY = lpScanSS[1].nStepVY; //  / lpnScanV[1]
			nScanLineEX = lpScanSS[1].nStartX;
			nScanLineEY = lpScanSS[1].nStartY;
			nPutStepEX = lpPutSS[1].nStepVX;	// / lpnScanV[1]
		}
		_asm {
			or		lpnScanU[1*4],0
			jz		conv_skip1
			//	�f�[�^�̌Œ菬���ϊ�
			mov		eax, nScanStepSX
			mov		edx, eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[1*4]
			mov		nScanStepSX, eax
			///////////////////////////////
			mov		eax, nScanStepSY
			mov		edx, eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[1*4]
			mov		nScanStepSY, eax
			//////////////////////////////
			sal		nScanLineSX,20
			sal		nScanLineSY,20
			/////////////////////////////
			mov		eax,nPutStepSX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[1*4]
			mov		nPutStepSX,eax
conv_skip1:
			or		lpnScanV[1*4],0
			jz		conv_skip2
			///////////////////////////////
			mov		eax,nScanStepEX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[1*4]
			mov		nScanStepEX, eax
			///////////////////////////////
			mov		eax,nScanStepEY
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[1*4]
			mov		nScanStepEY,eax
			//////////////////////////////
			sal		nScanLineEX,20
			sal		nScanLineEY,20
			////////////////////////////
			mov		eax,nPutStepEX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[1*4]
			mov		nPutStepEX,eax
conv_skip2:
			or		lpPutCount[1*4],0
			jz		end_ly2
			//	���������[�v
loop_ly2:
			//	�X�L�����J�n�I���ʒu�X�V
			mov		eax,nScanStepSX
			mov		ebx,nScanLineSX

			mov		edx,nScanStepSY
			sub		ebx,eax

			mov		ecx,nScanLineSY
			mov		nScanLineSX,ebx

			sub		ecx,edx
			mov		eax,nScanStepEX
			
			mov		nScanLineSY,ecx
			mov		ebx,nScanLineEX
			
			sub		ebx,eax
			mov		ecx,nScanLineEY
			
			mov		edx,nScanStepEY
			sub		ecx,edx
			
			mov		nScanLineEX,ebx
			mov		nScanLineEY,ecx
			
			//	�o�̓x�[�X���C���X�V���o�̓��C���X�V
			add		edi,lDstPitch
			mov		ecx,lDstPitch

			mov		lpDstBase,edi
			add		lpDstLineStart,ecx

			mov		eax,nPutStepSX
			mov		ebx,nPutLineSX

			sub		ebx,eax				//	EDI + (EBX >> 20) << 4���������݊J�n�ʒu
			add		lpDstLineEnd,ecx

			mov		edx,nPutStepEX
			mov		ecx,nPutLineEX

			mov		nPutLineSX,ebx
			sub		ecx,edx

			and		ebx,0xfff00000
			mov		nPutLineEX,ecx

			and		ecx,0xfff00000

			sub		ecx,ebx				//	ECX�ɃX�L�������C����(12:20)

			mov		nPutLineLeng, ecx
//			jc		line_skip3
			js		line_skip3				// ���������Ĕ�Ԃ悤�ɂɕύX���� '00.10.3.

			sar		ecx,20

			jz		line_skip3

			cmp		edi,lpDstStart		//	�o�̓��C���͈̓`�F�b�N
			jc		line_skip3

			cmp		edi,lpDstEnd
			jnc		line_skip3

			sar		ebx,20-2

			add		edi,ebx				//	edi���|�o�͊J�n�A�h���X
			//	�X�L�������C���ړ��䗦�����߂�
			cmp		ecx,1
			jbe		dclip_start2

			mov		eax, nPutLineAdj

			sub		nPutLineLeng,eax
			mov		eax,nScanLineSX

			mov		ebx,nScanLineEX

			sub		eax,ebx

			mov		edx,eax

			sal		eax,20
	
			sar		edx,12

			idiv	nPutLineLeng

			mov		nScanLineStepX,eax
			
			mov		eax,nScanLineSY
			mov		ebx,nScanLineEY

			sub		eax,ebx

			mov		edx,eax

			sal		eax,20

			sar		edx,12

			idiv	nPutLineLeng
			mov		nScanLineStepY,eax
dclip_start2:										//	�o�͂̉E�N���b�s���O
			lea		ebx,[edi+ecx*4]					//	bx <- �o�͏I���A�h���X
			sub		ebx,lpDstLineEnd
			jbe		dst_clip3
			sar		ebx,2
			sub		ecx,ebx
			jbe		line_skip3
dst_clip3:
			//	�X�L�����J�n�ʒu���o��	
			mov		ebx,nScanLineSX
			mov		eax,nScanLineSY
dst_clip4:											//	�o�͂̍��N���b�s���O
			cmp		edi,lpDstLineStart
			jnc		line_loop2

			sub		ebx,nScanLineStepX
			sub		eax,nScanLineStepY

			add		edi,4
			dec		ecx
			jz		line_skip3
			jmp		dst_clip4
line_loop2:
			mov		nScanPointX,ebx
			mov		nScanPointY,eax

			sar		eax,20
			mov		esi,lplpLineStart

			sar		ebx,20

			cmp		ebx,nSrcSX
			jc		line_skip4

			cmp		eax,nSrcSY
			jc		line_skip4
			
			cmp		ebx,nSrcEX
			jnc		line_skip4
			
			cmp		eax,nSrcEY
			jnc		line_skip4
			
			mov		esi,[esi+4*eax]
			mov		eax,nScanPointY

			mov		edx,[esi+4*ebx]
			mov		ebx,nScanPointX

			mov		[edi],edx
			sub		ebx,nScanLineStepX

			sub		eax,nScanLineStepY
			add		edi,4

			dec		ecx
			jnz		line_loop2

			jmp		line_skip3
line_skip4:	mov		eax,nScanPointY
			mov		ebx,nScanPointX
			//	�X�L�����ʒu�ړ�
			sub		ebx,nScanLineStepX
			sub		eax,nScanLineStepY
			//	�o�͈ʒu�ړ�
			add		edi,4
			dec		ecx
			jnz		line_loop2
line_skip3:
			mov		edi,lpDstBase
			//	���C���J�E���^�f�N�������g
			dec		lpPutCount[1 * 4]
			jnz		loop_ly2
end_ly2:
		}

		
		//	����������
	if (lpPutCount[2]) {
		if (lpnScanU[2]) {
			nScanStepSX = lpScanSS[2].nStepUX; //  / lpnScanU[2]
			nScanStepSY = lpScanSS[2].nStepUY; //  / lpnScanU[2]
			nScanLineSX = lpScanSS[2].nStartX;
			nScanLineSY = lpScanSS[2].nStartY;
			nPutStepSX = lpPutSS[2].nStepUX;	// / lpnScanU[2]
		}
		if (lpnScanV[2]) {
			nScanStepEX = lpScanSS[2].nStepVX; //  / lpnScanV[2]
			nScanStepEY = lpScanSS[2].nStepVY; //  / lpnScanV[2]
			nScanLineEX = lpScanSS[2].nStartX;
			nScanLineEY = lpScanSS[2].nStartY;
			nPutStepEX = lpPutSS[2].nStepVX;	// / lpnScanV[2]
		}
		_asm {
			or		lpnScanU[2*4],0
			jz		conv_skip3
			//	�f�[�^�̌Œ菬���ϊ�
			mov		eax, nScanStepSX
			mov		edx, eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[2*4]
			mov		nScanStepSX, eax
			///////////////////////////////
			mov		eax, nScanStepSY
			mov		edx, eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[2*4]
			mov		nScanStepSY, eax
			//////////////////////////////
			sal		nScanLineSX,20
			sal		nScanLineSY,20
			/////////////////////////////
			mov		eax,nPutStepSX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanU[2*4]
			mov		nPutStepSX,eax
conv_skip3:
			or		lpnScanV[2*4],0
			jz		conv_skip4
			///////////////////////////////
			mov		eax,nScanStepEX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[2*4]
			mov		nScanStepEX, eax
			///////////////////////////////
			mov		eax,nScanStepEY
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[2*4]
			mov		nScanStepEY,eax
			//////////////////////////////
			sal		nScanLineEX,20
			sal		nScanLineEY,20
			////////////////////////////
			mov		eax,nPutStepEX
			mov		edx,eax
			sar		edx,31
			sal		eax,20
			idiv	lpnScanV[2*4]
			mov		nPutStepEX,eax
conv_skip4:
			//	���������[�v
loop_ly3:
			//	�X�L�����J�n�I���ʒu�X�V
			mov		eax,nScanStepSX
			mov		ebx,nScanLineSX

			mov		edx,nScanStepSY
			sub		ebx,eax

			mov		ecx,nScanLineSY
			mov		nScanLineSX,ebx

			sub		ecx,edx
			mov		eax,nScanStepEX
			
			mov		nScanLineSY,ecx
			mov		ebx,nScanLineEX
			
			sub		ebx,eax
			mov		ecx,nScanLineEY
			
			mov		edx,nScanStepEY
			sub		ecx,edx
			
			mov		nScanLineEX,ebx
			mov		nScanLineEY,ecx
			
			//	�o�̓x�[�X���C���X�V���o�̓��C���X�V
			add		edi,lDstPitch
			mov		ecx,lDstPitch

			mov		lpDstBase,edi
			add		lpDstLineStart,ecx

			mov		eax,nPutStepSX
			mov		ebx,nPutLineSX

			sub		ebx,eax				//	EDI + (EBX >> 20) << 4���������݊J�n�ʒu
			add		lpDstLineEnd,ecx

			mov		edx,nPutStepEX
			mov		ecx,nPutLineEX

			mov		nPutLineSX,ebx
			sub		ecx,edx

			and		ebx,0xfff00000
			mov		nPutLineEX,ecx

			and		ecx,0xfff00000

			sub		ecx,ebx				//	ECX�ɃX�L�������C����(12:20)

			mov		nPutLineLeng, ecx
//			jc		line_skip5
			js		line_skip5				// ���������Ĕ�Ԃ悤�ɂɕύX���� '00.10.3.

			sar		ecx,20

			jz		line_skip5

			cmp		edi,lpDstStart		//	�o�̓��C���͈̓`�F�b�N
			jc		line_skip5

			cmp		edi,lpDstEnd
			jnc		line_skip5

			sar		ebx,20-2

			add		edi,ebx				//	edi���|�o�͊J�n�A�h���X
			//	�X�L�������C���ړ��䗦�����߂�
			cmp		ecx,1
			jbe		dclip_start3

			mov		eax, nPutLineAdj

			sub		nPutLineLeng,eax
			mov		eax,nScanLineSX

			mov		ebx,nScanLineEX

			sub		eax,ebx

			mov		edx,eax

			sal		eax,20
	
			sar		edx,12

			idiv	nPutLineLeng

			mov		nScanLineStepX,eax
			
			mov		eax,nScanLineSY
			mov		ebx,nScanLineEY

			sub		eax,ebx

			mov		edx,eax

			sal		eax,20

			sar		edx,12

			idiv	nPutLineLeng
			mov		nScanLineStepY,eax
dclip_start3:										//	�o�͂̉E�N���b�s���O
			lea		ebx,[edi+ecx*4]					//	bx <- �o�͏I���A�h���X
			sub		ebx,lpDstLineEnd
			jbe		dst_clip5
			sar		ebx,2
			sub		ecx,ebx
			jbe		line_skip5
dst_clip5:
			//	�X�L�����J�n�ʒu���o��	
			mov		ebx,nScanLineSX
			mov		eax,nScanLineSY
dst_clip6:											//	�o�͂̍��N���b�s���O
			cmp		edi,lpDstLineStart
			jnc		line_loop3

			sub		ebx,nScanLineStepX
			sub		eax,nScanLineStepY

			add		edi,4
			dec		ecx
			jz		line_skip5
			jmp		dst_clip6
line_loop3:
			mov		nScanPointX,ebx
			mov		nScanPointY,eax

			sar		eax,20
			mov		esi,lplpLineStart

			sar		ebx,20

			cmp		ebx,nSrcSX
			jc		line_skip6

			cmp		eax,nSrcSY
			jc		line_skip6
			
			cmp		ebx,nSrcEX
			jnc		line_skip6
			
			cmp		eax,nSrcEY
			jnc		line_skip6
			
			mov		esi,[esi+4*eax]
			mov		eax,nScanPointY

			mov		edx,[esi+4*ebx]
			mov		ebx,nScanPointX

			mov		[edi],edx
			sub		ebx,nScanLineStepX

			sub		eax,nScanLineStepY
			add		edi,4

			dec		ecx
			jnz		line_loop3

			jmp		line_skip5
line_skip6:	mov		eax,nScanPointY
			mov		ebx,nScanPointX
			//	�X�L�����ʒu�ړ�
			sub		ebx,nScanLineStepX
			sub		eax,nScanLineStepY
			//	�o�͈ʒu�ړ�
			add		edi,4
			dec		ecx
			jnz		line_loop3
line_skip5:
			mov		edi,lpDstBase
			//	���C���J�E���^�f�N�������g
			dec		lpPutCount[2 * 4]
			jnz		loop_ly3
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////
//	RotateBlt

LRESULT CDIB32P5::RotateBlt(CDIB32* lpDIB32,LPRECT lpSrcRect,int x,int y,int nAngle,int nRate,int nType,LPRECT lpClip){
	int sx,sy;	//	�]�����T�C�Y
	if (lpSrcRect==NULL) {
		lpDIB32->GetSize(sx,sy);
	} else {
		sx = lpSrcRect->right - lpSrcRect->left;
		sy = lpSrcRect->bottom - lpSrcRect->top;
	}
	int dx,dy;	//	�]����T�C�Y
	dx = ::MulDiv(sx,nRate,1<<16);
	dy = ::MulDiv(sy,nRate,1<<16);
	int px,py;	//	��]���S
	switch (nType){
	case 0: px = 0;		py = 0;		break;	//	����
	case 1: px = dx;	py = 0;		break;	//	�E��
	case 2: px = 0;		py = dy;	break;	//	����
	case 3: px = dx;	py = dy;	break;	//	�E��
	case 4: px = dx>>1; py = dy>>1;	break;	//	�摜���S
	}
	px+=x; py+=y;
	POINT point[4];
	point[0].x = x;		point [0].y = y;
	point[1].x = x+dx;	point [1].y = y;
	point[2].x = x;		point [2].y = y+dy;
	point[3].x = x+dx;	point [3].y = y+dy;
	static CSinTable st;
	nAngle = -nAngle;	//	y���͉��ɂƂ�̂ŉ�]�����͋t�ɂȂ�
	for(int i=0;i<4;++i){
		//	(px,py)���S�̉�]�Ȃ̂ŁA(px,py)�����_�ɕ��s�ړ������āA���_���S�ɉ�]���������ƁA(px,py)�������s�ړ�
		int ax = point[i].x-px , ay = point[i].y-py;
		point[i].x	=  ((ax * st.Cos(nAngle) - ay * st.Sin(nAngle))>>16)+px;
		point[i].y	=  ((ax * st.Sin(nAngle) + ay * st.Cos(nAngle))>>16)+py;
	}
	return MorphBlt(lpDIB32,lpSrcRect,&point[0],lpClip);
}

LRESULT CDIB32P5::RotateBltFast(CDIB32* lpDIB32,LPRECT lpSrcRect,int x,int y,int nAngle,int nRate,int nType,LPRECT lpClip){
	int sx,sy;	//	�]�����T�C�Y
	if (lpSrcRect==NULL) {
		lpDIB32->GetSize(sx,sy);
	} else {
		sx = lpSrcRect->right - lpSrcRect->left;
		sy = lpSrcRect->bottom - lpSrcRect->top;
	}
	int dx,dy;	//	�]����T�C�Y
	dx = ::MulDiv(sx,nRate,1<<16);
	dy = ::MulDiv(sy,nRate,1<<16);
	int px,py;	//	��]���S
	switch (nType){
	case 0: px = 0;		py = 0;		break;	//	����
	case 1: px = dx;	py = 0;		break;	//	�E��
	case 2: px = 0;		py = dy;	break;	//	����
	case 3: px = dx;	py = dy;	break;	//	�E��
	case 4: px = dx>>1; py = dy>>1;	break;	//	�摜���S
	}
	px+=x; py+=y;
	POINT point[4];
	point[0].x = x;		point [0].y = y;
	point[1].x = x+dx;	point [1].y = y;
	point[2].x = x;		point [2].y = y+dy;
	point[3].x = x+dx;	point [3].y = y+dy;
	static CSinTable st;
	nAngle = -nAngle;	//	y���͉��ɂƂ�̂ŉ�]�����͋t�ɂȂ�
	for(int i=0;i<4;++i){
		//	(px,py)���S�̉�]�Ȃ̂ŁA(px,py)�����_�ɕ��s�ړ������āA���_���S�ɉ�]���������ƁA(px,py)�������s�ړ�
		int ax = point[i].x-px , ay = point[i].y-py;
		point[i].x	=  ((ax * st.Cos(nAngle) - ay * st.Sin(nAngle))>>16)+px;
		point[i].y	=  ((ax * st.Sin(nAngle) + ay * st.Cos(nAngle))>>16)+py;
	}
	return MorphBltFast(lpDIB32,lpSrcRect,&point[0],lpClip);
}

#endif