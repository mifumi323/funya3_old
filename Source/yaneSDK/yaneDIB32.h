//	Device Independent Bitmap

#ifdef USE_DIB32

#ifndef __yaneDIB32_h__
#define __yaneDIB32_h__

#include "yanePlaneBase.h"

#ifdef USE_DirectDraw
class CPlane;
#endif
/*
	DirectDrawSurface�Ƃ̐e�a�����v��Ȃ�΁A
		//	DIB32��GDI�Ƃ͈قȂ�A���ʂ���B,G,R,��
		//	CreateDIBSection�ō�����ꍇ���A���ʂ���B,G,R,���B
		//	���Ȃ݂�GDI�́A���ʂ���R,G,B,0(COLORREF)�BRGB(r,g,b)�}�N�����g����B
*/
//	#define DIB32RGB(r,g,b) ( ((DWORD)r)<<16 | ((DWORD)g)<<8 | b )
//	�����ꂾ���Ar,g,b<=255���`�F�b�N�����ق������S�Ȃ̂ŁA���������R�[�h�ɂ���B
inline DWORD DIB32RGB(DWORD r,DWORD g,DWORD b){
	WARNING(r>=256 || g>=256 || b>=256,"DIB32RGB�Œl���I�[�o�[���Ă��܂�");
	return ( ((DWORD)r)<<16 | ((DWORD)g)<<8 | b );
}

class CDIB32;
class CDIB32P5;
class CDIB32PMMX;
class CDIB32P6;

class CDIB32Base {

	//	����Ȃ񏑂���Ƃ����񂩁[�B�_�T���́[�B
	friend class CDIB32P5;
	friend class CDIB32PMMX;
	friend class CDIB32P6;
	friend class CDIB32Effect;

public:
	////////////////////////////////////////////////////////
	virtual LRESULT Load(string szBitmapFileName,bool bLoadPalette=false);

	LRESULT Save(string BitmapFileName,LPRECT lpRect=NULL);

#ifdef USE_YGA
	LRESULT SaveYGA(string YGAFileName,LPRECT lpRect=NULL,bool bCompress = true);
#endif

	LRESULT CreateSurface(int nSizeX,int nSizeY);
	LRESULT Resize(int nSizeX,int nSizeY);
	LRESULT	Release(void);
	bool	IsLoad(void) const;

	////////////////////////////////////////////////////////
	//	CreateDIBSection���g���̂��H

	void	UseDIBSection(bool bUseDIB);
	//	----- CreateDIBSection���g���Ƃ��Ɍ���A�ȉ��̊֐����g�p�\ -----

	//	HDC�擾(����͕s�v�BLoad��CreateSurface�������Ƃ͉������܂Ŏ擾�\)
	HDC		GetDC(void);
	HBITMAP	GetHBITMAP(void);

#ifdef USE_DirectDraw
	LRESULT	BltToPlane(CPlane*lpDstPlane,int x,int y,LPRECT lpSrcRect=NULL,LPRECT lpClipRect=NULL);
	LRESULT	BltFromPlane(CPlane*lpSrcPlane,int x,int y,LPRECT lpSrcRect=NULL,LPRECT lpClipRect=NULL);
#endif

	////////////////////////////////////////////////////////

	// ���߃L�[�ݒ�֘A
	LRESULT SetColorKey(DWORD dwDIB32RGB);	// ����̐F��]���̂Ƃ��̓��߃L�[�ɐݒ肷��
	LRESULT SetColorKey(int x,int y);	// (x,y)�̓_�𓧉߃L�[�ɐݒ肷��
	LRESULT SetColorKey(int r,int g,int b);
	DWORD	GetColorKey(void);

	//	��`�`��
	virtual LRESULT Blt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BltFast(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;

	virtual LRESULT BlendBltHalf(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BlendBltFastHalf(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;

	virtual LRESULT BlendBlt(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BlendBltFast(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;

	virtual LRESULT AddColorBlt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT AddColorBltFast(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT SubColorBlt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT SubColorBltFast(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;

	virtual LRESULT BltClearAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BltFastClearAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BlendBltFastAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BlendBltFastAlpha(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BltWithoutAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BltFastWithoutAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;

	// �~���[�t����`�`��
	virtual LRESULT BltM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BltFastM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BlendBltHalfM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BlendBltFastHalfM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BlendBltM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BlendBltFastM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT AddColorBltM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT AddColorBltFastM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT SubColorBltM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT SubColorBltFastM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;

	virtual LRESULT BltClearAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BltFastClearAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BlendBltFastAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BlendBltFastAlphaM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BltWithoutAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BltFastWithoutAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;

	virtual LRESULT BltNatural(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltNatural(CDIB32* lpDIBSrc32,int x,int y,int nFade,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual	bool IsYGA(void) { return m_bYGA; }
	virtual bool* GetYGA(void) { return &m_bYGA; }

	virtual LRESULT BltToAlpha(CDIB32* lpSrc,int nSrcMin,int nSrcMax,int nDstMin=0,int nDstMax=255,LPRECT lpRECT=NULL)=0;
	virtual LRESULT FadeBltAlpha(CDIB32* lpSrc,int nFadeRate,LPRECT lpRect=NULL)=0;

	virtual LRESULT MorphBlt(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPRECT lpDstRect, LPRECT lpClip = NULL) = 0;
	virtual LRESULT MorphBlt(CDIB32* lpDIB32, LPRECT lpSrcRect, LPPOINT lpDstPoint, LPRECT lpClip = NULL) = 0;
	virtual LRESULT MorphBlt(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPPOINT lpDstPoint, LPRECT lpClip = NULL, bool bContinual = false) = 0;
	virtual LRESULT MorphBltFast(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPRECT lpDstRect, LPRECT lpClip = NULL) = 0;
	virtual LRESULT MorphBltFast(CDIB32* lpDIB32, LPRECT lpSrcRect, LPPOINT lpDstPoint, LPRECT lpClip = NULL) = 0;
	virtual LRESULT MorphBltFast(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPPOINT lpDstPoint, LPRECT lpClip = NULL, bool bContinual = false) = 0;

	virtual LRESULT RotateBlt(CDIB32* lpDIB32,LPRECT lpSrcRect,int x,int y,int nAngle,int nRate,int nType=0,LPRECT lpClip=NULL) = 0;
	virtual LRESULT RotateBltFast(CDIB32* lpDIB32,LPRECT lpSrcRect,int x,int y,int nAngle,int nRate,int nType=0,LPRECT lpClip=NULL) = 0;

	//	���̃v���[���ɑ΂���G�t�F�N�g

	virtual LRESULT AddColorFast(DWORD dwAddRGB,LPRECT lpSrcRect=NULL) = 0;
	virtual LRESULT SubColorFast(DWORD dwSubRGB,LPRECT lpSrcRect=NULL) = 0;
	virtual LRESULT ShadeOff(LPRECT lpSrcRect=NULL) = 0;
	virtual LRESULT MosaicEffect(int d, LPRECT lpRect=NULL) = 0;
	virtual LRESULT FlushEffect(LPRECT lpRect=NULL) = 0;
	virtual LRESULT FadeEffect(int nFade,LPRECT lpRect=NULL) = 0;
	virtual LRESULT FadeAlpha(int nFade,LPRECT lpRect=NULL) = 0;
	virtual LRESULT FlushAlpha(LPRECT lpRect=NULL) = 0;

	//	�v���[���̃s�N�Z������
	DWORD	GetPixel(int x,int y);						//	�s�N�Z���擾
	void	SetPixel(int x,int y,DWORD dwDIB32RGB);	//	�s�N�Z���ݒ�

	//	���̒��o(��YGA�摜�̂Ƃ��͔����F�ȊO�Ȃ��255)
	virtual int	GetPixelAlpha(int x,int y);

	void	GetSize(int& sx,int& sy);					//	�v���[���T�C�Y�̎擾
	LPRECT	GetRect(void);								//	���̃v���[���̃T�[�t�F�[�X��`�擾
	DWORD*	GetPtr(void);								//	�T�[�t�F�[�X�ւ̃|�C���^���擾
	RECT	GetClipRect(LPRECT lpRect);					//	���̃T�[�t�F�[�X��Clip���ꂽRect��Ԃ�

	LRESULT	Clear(DWORD dwDIB32RGB=CLR_INVALID,LPRECT lpRect=NULL);
	void	SetFillColor(DWORD dwDIB32RGB);		//	Clear����F�̐ݒ�

	CDIB32Base();
	virtual ~CDIB32Base();

protected:
	LRESULT InnerCreateSurface(int nSizeX,int nSizeY);
	void	ClearMSByte(void);	//	�ŏ�ʃo�C�g�̃N���A

#ifdef USE_YGA
	LRESULT	InnerLoadYGA(string BitmapFileName);	//	YGA�̃��[�h���[�`��
#endif

	BYTE*	m_lpdwSrcOrg;	//	�m�ۂ���DIB������
	DWORD*	m_lpdwSrc;		//	�m�ۂ������̂�QWORD�ŃA���C����������
	RECT	m_rcRect;		//	DIB�̃T�C�Y��(m_rcRect.right,m_rcRect.bottom)
	auto_array<DWORD*> m_lpdwLineStart;	//	�e���C���̐擪�A�h���X�e�[�u��
	LONG	m_lPitch;		//	�P���C���̃o�C�g��(�ʗ�m_nSizeX*4)
	//	�����Pixel(x,y) = (BYTE*)m_lpdwSrc + x + y * m_lPitch

	DWORD	m_dwColorKey;	//	�J���[�L�[
	DWORD	m_dwFillColor;	//	Clear����Ƃ��̐F
	
	//----------------
	LRESULT InnerLoad(string BitmapFileName); // DIBSection���g���Ă���Ƃ��p
	bool	m_bUseDIBSection;	//	DIBSection���g���̂�
	HBITMAP m_hBitmap;
	HDC		m_hDC;
	bool	m_bLoadBitmap;		//	�r�b�g�}�b�v��ǂݍ��ݒ��Ȃ̂��H

	///////////////////////////////////////////////
	LRESULT TestClipping(LPRECT lpRectDstDIB, LPRECT lpRectSrcDIB, LPRECT lpRectSrc, LPPOINT lpDstPoint, LPRECT lpClip=NULL);
	
	///////////////////////////////////////////////////////////////////////////
	//	�ėp�N���b�s���O���[�`�� (c) yaneurao

	LRESULT	Clipping(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect);
	LRESULT	ClippingM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect);
	//	��0:�]���͈͂Ȃ��Ȃ̂ŁA���̂܂܋A��ׂ�
	//	0:	m_bActualSize==true
	//		���{�̓]��			�]�����` m_rcDstRect
	//							�]������`:m_rcSrcRect
	//		m_bMirror���^�Ȃ�΍��E�~���[
	//	0:	m_dbActualSize==false
	//		�g��k�����݂̓]��	�]�����` m_rcDstRect
	//							�]�������W�@��`�̍���:(m_rcSrcRect.left,m_rcSrcRect.top)
	//		m_bMirror���^�Ȃ�΍��E�~���[
	//							�]�������W�@��`�̉E��:(m_rcSrcRect.right,m_rcSrcRect.top)
	//		���@���̂Ƃ��̃u���[���n���̏����l,����,�������]���Ɉ����ʂ����ꂼ��
	//		m_nInitial,m_nStep,m_nCmp�ɓ���B

	RECT	m_rcSrcRect;
	RECT	m_rcDstRect;
	bool	m_bMirror;		//	�~���[���H
	bool	m_bActualSize;	//	�����傩�H

	int		m_nInitialX;	//	-DX�@ :�@�Â̏����l = -DX
	int		m_nStepX;		//	 2*SX :�@��+=2*SX
	int		m_nCmpX;		//	 2*DX :�@��>0�Ȃ�΃�-=2*DX���Ă�
	int		m_nStepsX;		//	 SrcX�̈��̉��Z��(������)

	int		m_nInitialY;
	int		m_nStepY;
	int		m_nCmpY;
	int		m_nStepsY;

	//	�N���b�v�L���Morph���[�`��
	// Src��point�� 1�ӂł��͂ݏo���Ă����ꍇ�A���̃t���O�� true �ɂȂ�
	bool	m_bSrcClip;		// Src���N���b�s���O��v���邩�H

	bool	m_bYGA;			//	�x�f�`�摜�������Ă���̂��H

#ifdef USE_YGA
	//	YPG�̃w�b�_�[
	struct SYPGHeader {
		DWORD dwIdentifier;
		DWORD dwSizeX;
		DWORD dwSizeY;
		DWORD bCompress;
		DWORD dwOriginalSize;
		DWORD dwCompressSize;
		SYPGHeader() { dwIdentifier = 0x616779; /* "yga" */ }
	};
#endif
};

///////////////////////////////////////////////////////////////////////////////


//	Pentium�p�R�[�h
class CDIB32P5 : public CDIB32Base {
public:
	// ��`�`��
	virtual LRESULT Blt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltFast(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltHalf(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastHalf(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBlt(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFast(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT AddColorBlt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT AddColorBltFast(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT SubColorBlt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT SubColorBltFast(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);

	virtual LRESULT AddColorFast(DWORD dwAddRGB,LPRECT lpSrcRect=NULL);
	virtual LRESULT SubColorFast(DWORD dwSubRGB,LPRECT lpSrcRect=NULL);
	virtual LRESULT ShadeOff(LPRECT lpSrcRect=NULL);
	virtual LRESULT MosaicEffect(int d, LPRECT lpRect=NULL);
	virtual LRESULT FlushEffect(LPRECT lpRect=NULL);
	virtual LRESULT FadeEffect(int nFade,LPRECT lpRect=NULL);
	virtual LRESULT FadeAlpha(int nFade,LPRECT lpRect=NULL);
	virtual LRESULT FlushAlpha(LPRECT lpRect=NULL);

	virtual LRESULT BltClearAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltFastClearAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastAlpha(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltWithoutAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltFastWithoutAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);

	// �~���[�t����`�`��
	virtual LRESULT BltM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltFastM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltHalfM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastHalfM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT AddColorBltM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT AddColorBltFastM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT SubColorBltM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT SubColorBltFastM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);

	virtual LRESULT BltClearAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltFastClearAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastAlphaM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltWithoutAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltFastWithoutAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);

	virtual LRESULT BltToAlpha(CDIB32* lpSrc,int nSrcMin,int nSrcMax,int nDstMin=0,int nDstMax=255,LPRECT lpRECT=NULL);
	virtual LRESULT FadeBltAlpha(CDIB32* lpSrc,int nFadeRate,LPRECT lpRect=NULL);

	virtual LRESULT MorphBlt(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPRECT lpDstRect, LPRECT lpClip = NULL);
	virtual LRESULT MorphBlt(CDIB32* lpDIB32, LPRECT lpSrcRect, LPPOINT lpDstPoint, LPRECT lpClip = NULL);
	virtual LRESULT MorphBlt(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPPOINT lpDstPoint, LPRECT lpClip = NULL, bool bContinual = false);
	virtual LRESULT MorphBltFast(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPRECT lpDstRect, LPRECT lpClip = NULL);
	virtual LRESULT MorphBltFast(CDIB32* lpDIB32, LPRECT lpSrcRect, LPPOINT lpDstPoint, LPRECT lpClip = NULL);
	virtual LRESULT MorphBltFast(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPPOINT lpDstPoint, LPRECT lpClip = NULL, bool bContinual = false);
	virtual LRESULT RotateBlt(CDIB32* lpDIB32,LPRECT lpSrcRect,int x,int y,int nAngle,int nRate,int nType=0,LPRECT lpClip=NULL);
	virtual LRESULT RotateBltFast(CDIB32* lpDIB32,LPRECT lpSrcRect,int x,int y,int nAngle,int nRate,int nType=0,LPRECT lpClip=NULL);

protected:
};

//	MMX�p�R�[�h
class CDIB32PMMX : public CDIB32P5 {
	//	�K�v�Ȋ֐����I�[�o�[���C�h����
public:
	// ��`�`��
	virtual LRESULT Blt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltFast(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltHalf(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastHalf(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBlt(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFast(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT AddColorBlt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT AddColorBltFast(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT SubColorBlt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT SubColorBltFast(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT AddColorFast(DWORD dwAddRGB,LPRECT lpSrcRect=NULL);
	virtual LRESULT SubColorFast(DWORD dwSubRGB,LPRECT lpSrcRect=NULL);
	virtual LRESULT BltClearAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltFastClearAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastAlpha(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltWithoutAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltFastWithoutAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);

	// �~���[�t����`�`��
	virtual LRESULT BltM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltFastM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltHalfM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastHalfM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT AddColorBltM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT AddColorBltFastM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT SubColorBltM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT SubColorBltFastM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltClearAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltFastClearAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastAlphaM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltWithoutAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltFastWithoutAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	//
protected:
};
//	PentiumPro,II�p�R�[�h
class CDIB32P6 : public CDIB32PMMX {
	//	�K�v�Ȋ֐����I�[�o�[���C�h����
/*	virtual LRESULT Blt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltHalfM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFastHalf(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT SubColorBlt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT SubColorBltM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT SubColorBltFast(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT SubColorBltFastM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
*/
};

////////////////////////////////////////////////////////////////////////////////////////

//	�����CDIB32Base�̑㗝��i�f�U�C���p�^�[���Ō����Ƃ����proxy�p�^�[���j
class CDIB32 : public CPlaneBase {
public:

	//	CPlaneBase�̔h���N���X�ł́A������I�[�o�[���C�h���ׂ�
	virtual EDrawType GetID() const { return eDraw_CDIB32; }

	virtual LRESULT Load(string BitmapFileName,bool bLoadPalette=false) {
		return m_lpDIB->Load(BitmapFileName,bLoadPalette);
	}
	LRESULT Save(string BitmapFileName,LPRECT lpRect=NULL){ return m_lpDIB->Save(BitmapFileName,lpRect); }

#ifdef USE_YGA
	LRESULT SaveYGA(string BitmapFileName,LPRECT lpRect=NULL, bool bCompress = true ){ return m_lpDIB->SaveYGA(BitmapFileName,lpRect,bCompress); }
#endif

	//	CDIB32�ɂ�YGA�̊T�O�͊֌W�����̂ŁA���̃t���O�͖������ėǂ�
	virtual LRESULT CreateSurface(int nSizeX,int nSizeY,bool bYGA=false) { return m_lpDIB->CreateSurface(nSizeX,nSizeY); }
	LRESULT Resize(int nSizeX,int nSizeY) { return m_lpDIB->Resize(nSizeX,nSizeY); }
	virtual LRESULT	Release(void) { return m_lpDIB->Release(); }
	bool	IsLoad(void) const { return m_lpDIB->IsLoad(); }
	virtual bool IsLoaded(void) const { return m_lpDIB->IsLoad(); }

	// ���߃L�[�ݒ�֘A
	LRESULT SetColorKey(DWORD dwDIB32RGB) { return m_lpDIB->SetColorKey(dwDIB32RGB); }	// ����̐F��]���̂Ƃ��̓��߃L�[�ɐݒ肷��
	virtual LRESULT SetColorKey(int x,int y){ return m_lpDIB->SetColorKey(x,y); }	// (x,y)�̓_�𓧉߃L�[�ɐݒ肷��
	virtual LRESULT SetColorKey(int r,int g,int b){ return m_lpDIB->SetColorKey(r,g,b); }	// (x,y)�̓_�𓧉߃L�[�ɐݒ肷��
	DWORD	GetColorKey(void)				{	return m_lpDIB->GetColorKey(); }	//	�J���[�L�[�̎擾

	////////////////////////////////////////////////////////
	//	CreateDIBSection���g���̂��H

	void	UseDIBSection(bool bUseDIB)	{ m_lpDIB->UseDIBSection(bUseDIB); }

	//	CreateDIBSection���g���Ƃ��Ɍ���A�ȉ��̊֐����g�p�\
	HDC		GetDC(void)					{ return m_lpDIB->GetDC(); }
	HBITMAP GetHBITMAP(void)			{ return m_lpDIB->GetHBITMAP(); }
	//	GetDC�Ƃ̒��덇�킹�B(CDIBDraw��CDirectDraw�̗����œ����N���X�݌v������Ƃ��̂��߂�)
	LRESULT	ReleaseDC(void)				{ return 0; } // �������܂�����O�O�G

#ifdef USE_DirectDraw
	//	HDC�擾(����͕s�v�BLoad��CreateSurface�������Ƃ͉������܂Ŏ擾�\)
	LRESULT	BltToPlane(CPlane*lpSrcPlane,int x,int y,LPRECT lpSrcRect=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BltToPlane(lpSrcPlane,x,y,lpSrcRect,lpClipRect);
	}
	LRESULT	BltFromPlane(CPlane*lpSrcPlane,int x,int y,LPRECT lpSrcRect=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BltFromPlane(lpSrcPlane,x,y,lpSrcRect,lpClipRect);
	}
#endif
	////////////////////////////////////////////////////////

	//	��`�`��
	LRESULT Blt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL
		,LPRECT lpClipRect=NULL){
		return m_lpDIB->Blt(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT BltFast(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL
		,LPRECT lpClipRect=NULL) {
		return m_lpDIB->BltFast(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT BlendBltHalf(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL
		,LPRECT lpClipRect=NULL) {
		return m_lpDIB->BlendBltHalf(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT BlendBltFastHalf(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL
		,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		return m_lpDIB->BlendBltFastHalf(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT BlendBlt(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BlendBlt(lpDIBSrc32,x,y,dwDstRGBRate,dwSrcRGBRate,lpSrcRect
			,lpDstSize,lpClipRect);
	}

	LRESULT BlendBltFast(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BlendBltFast(lpDIBSrc32,x,y,dwDstRGBRate,dwSrcRGBRate
				,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT AddColorBlt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->AddColorBlt(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT AddColorBltFast(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->AddColorBltFast(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT SubColorBlt(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->SubColorBlt(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT SubColorBltFast(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->SubColorBltFast(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT BltClearAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BltClearAlpha(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}
	LRESULT BltFastClearAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BltFastClearAlpha(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}
	//	override from CPlane
	virtual LRESULT BlendBltFastAlpha(CPlaneBase* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BlendBltFastAlpha((CDIB32*)lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}
	virtual LRESULT BlendBltFastAlpha(CPlaneBase* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BlendBltFastAlpha((CDIB32*)lpDIBSrc32,x,y,dwDstRGBRate,dwSrcRGBRate
				,lpSrcRect,lpDstSize,lpClipRect);
	}
	LRESULT BltWithoutAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL
		,LPRECT lpClipRect=NULL){
		return m_lpDIB->BltWithoutAlpha(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT BltFastWithoutAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL
		,LPRECT lpClipRect=NULL) {
		return m_lpDIB->BltFastWithoutAlpha(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	//	�~���[�t����`�`��
	LRESULT BltM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL
		,LPRECT lpClipRect=NULL){
		return m_lpDIB->BltM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT BltFastM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL
		,LPRECT lpClipRect=NULL) {
		return m_lpDIB->BltFastM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT BlendBltHalfM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL
		,LPRECT lpClipRect=NULL) {
		return m_lpDIB->BlendBltHalfM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT BlendBltFastHalfM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL
		,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		return m_lpDIB->BlendBltFastHalfM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT BlendBltM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BlendBltM(lpDIBSrc32,x,y,dwDstRGBRate,dwSrcRGBRate,lpSrcRect
			,lpDstSize,lpClipRect);
	}

	LRESULT BlendBltFastM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BlendBltFastM(lpDIBSrc32,x,y,dwDstRGBRate,dwSrcRGBRate
				,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT AddColorBltM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->AddColorBltM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT AddColorBltFastM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->AddColorBltFastM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT SubColorBltM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->SubColorBltM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT SubColorBltFastM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->SubColorBltFastM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT BltClearAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BltClearAlphaM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}
	LRESULT BltFastClearAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BltFastClearAlphaM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}
	LRESULT BlendBltFastAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BlendBltFastAlphaM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}
	LRESULT BlendBltFastAlphaM(CPlaneBase* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return m_lpDIB->BlendBltFastAlphaM((CDIB32*)lpDIBSrc32,x,y,dwDstRGBRate,dwSrcRGBRate
				,lpSrcRect,lpDstSize,lpClipRect);
	}
	LRESULT BltWithoutAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL
		,LPRECT lpClipRect=NULL){
		return m_lpDIB->BltWithoutAlphaM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	LRESULT BltFastWithoutAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL
		,LPRECT lpClipRect=NULL) {
		return m_lpDIB->BltFastWithoutAlphaM(lpDIBSrc32,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}

	virtual LRESULT BltToAlpha(CDIB32* lpSrc,int nSrcMin,int nSrcMax,int nDstMin=0,int nDstMax=255,LPRECT lpRect=NULL){
		return m_lpDIB->BltToAlpha(lpSrc,nSrcMin,nSrcMax,nDstMin,nDstMax,lpRect);
	}
	virtual LRESULT FadeAlpha(int nFade,LPRECT lpRect=NULL){
		return m_lpDIB->FadeAlpha(nFade,lpRect);
	}

	//	����T�C�Y�̃v���[���łȂ��Ƃ����Ȃ��O�O�G
	virtual LRESULT FadeBltAlpha(CDIB32* lpSrc,int nFade,LPRECT lpRect=NULL){
		return m_lpDIB->FadeBltAlpha(lpSrc,nFade,lpRect);
	}

	LRESULT MorphBlt(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPRECT lpDstRect, LPRECT lpClip = NULL) {
		return m_lpDIB->MorphBlt(lpDIB32, lpSrcPoint, lpDstRect, lpClip);
	}
	LRESULT MorphBlt(CDIB32* lpDIB32, LPRECT lpSrcRect, LPPOINT lpDstPoint, LPRECT lpClip = NULL) {
		return m_lpDIB->MorphBlt(lpDIB32, lpSrcRect, lpDstPoint, lpClip);
	}
	LRESULT MorphBlt(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPPOINT lpDstPoint, LPRECT lpClip = NULL, bool bContinual = false){
		return m_lpDIB->MorphBlt(lpDIB32, lpSrcPoint, lpDstPoint, lpClip, bContinual);
	}

	LRESULT MorphBltFast(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPRECT lpDstRect, LPRECT lpClip = NULL) {
		return m_lpDIB->MorphBltFast(lpDIB32, lpSrcPoint, lpDstRect, lpClip);
	}
	LRESULT MorphBltFast(CDIB32* lpDIB32, LPRECT lpSrcRect, LPPOINT lpDstPoint, LPRECT lpClip = NULL) {
		return m_lpDIB->MorphBltFast(lpDIB32, lpSrcRect, lpDstPoint, lpClip);
	}
	LRESULT MorphBltFast(CDIB32* lpDIB32, LPPOINT lpSrcPoint, LPPOINT lpDstPoint, LPRECT lpClip = NULL, bool bContinual = false){
		return m_lpDIB->MorphBltFast(lpDIB32, lpSrcPoint, lpDstPoint, lpClip, bContinual);
	}

	LRESULT RotateBlt(CDIB32* lpDIB32,LPRECT lpSrcRect,int x,int y,int nAngle,int nRate,int nType=0,LPRECT lpClip=NULL){
		return m_lpDIB->RotateBlt(lpDIB32,lpSrcRect,x,y,nAngle,nRate,nType,lpClip);
	}
	LRESULT RotateBltFast(CDIB32* lpDIB32,LPRECT lpSrcRect,int x,int y,int nAngle,int nRate,int nType=0,LPRECT lpClip=NULL){
		return m_lpDIB->RotateBltFast(lpDIB32,lpSrcRect,x,y,nAngle,nRate,nType,lpClip);
	}


	//	���̃v���[���ɑ΂���G�t�F�N�g

	LRESULT AddColorFast(DWORD dwAddRGB,LPRECT lpSrcRect=NULL){
		return m_lpDIB->AddColorFast(dwAddRGB,lpSrcRect);
	}
	LRESULT SubColorFast(DWORD dwSubRGB,LPRECT lpSrcRect=NULL){
		return m_lpDIB->SubColorFast(dwSubRGB,lpSrcRect);
	}
	LRESULT ShadeOff(LPRECT lpSrcRect=NULL) {
		return m_lpDIB->ShadeOff(lpSrcRect);
	}
	virtual LRESULT MosaicEffect(int d, LPRECT lpRect=NULL){
		return m_lpDIB->MosaicEffect(d,lpRect);
	}
	virtual LRESULT FlushEffect(LPRECT lpRect=NULL){
		return m_lpDIB->FlushEffect(lpRect);
	}
	virtual LRESULT FadeEffect(int nFade,LPRECT lpRect=NULL){
		return m_lpDIB->FadeEffect(nFade,lpRect);
	}
	virtual LRESULT FlushAlpha(LPRECT lpRect=NULL){
		return m_lpDIB->FlushAlpha(lpRect);
	}

	//	�v���[���̃s�N�Z������
	DWORD	GetPixel(int x,int y){	return m_lpDIB->GetPixel(x,y); }
	void	SetPixel(int x,int y,DWORD dwDIB32RGB) { m_lpDIB->SetPixel(x,y,dwDIB32RGB); }
	virtual void GetSize(int& sx,int& sy) { m_lpDIB->GetSize(sx,sy); }
	LPRECT	GetRect(void) { return m_lpDIB->GetRect(); }
	DWORD*	GetPtr(void) { return m_lpDIB->GetPtr(); }
	RECT	GetClipRect(LPRECT lpRect) { return m_lpDIB->GetClipRect(lpRect); }

	LRESULT	Clear(DWORD dwDIB32RGB=CLR_INVALID,LPRECT lpRect=NULL){ return m_lpDIB->Clear(dwDIB32RGB,lpRect); }
	void	SetFillColor(DWORD dwDIB32RGB){ m_lpDIB->SetFillColor(dwDIB32RGB); }	//	Clear����F�̐ݒ�

	//	YGA���������߂̃t���O
	bool	IsYGA(void) { return m_lpDIB->IsYGA(); }
	bool*	GetYGA(void) { return m_lpDIB->GetYGA(); }
	//	���R�ȓ]��
	virtual LRESULT BltNatural(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		return m_lpDIB->BltNatural(reinterpret_cast<CDIB32*>(lpSrc),x,y,lpSrcRect,lpDstSize,lpClipRect);
	}
	virtual LRESULT BltNatural(CPlaneBase* lpSrc,int x,int y,int nFadeRate,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		return m_lpDIB->BltNatural(reinterpret_cast<CDIB32*>(lpSrc),x,y,nFadeRate,lpSrcRect,lpDstSize,lpClipRect);
	}

	CDIB32();
	virtual ~CDIB32();

	//	���܂�
	CDIB32Base* GetDIB32BasePtr(void) { return m_lpDIB; }

protected:
	auto_ptrEx<CDIB32Base>	m_lpDIB;	//	������ێ�

	//////////////////////////////////////////////////////////////////////////
	//	override from CPlaneBase
	//	�����́A���ڌĂяo����邱�Ƃ͖����̂�protected�ɂ��Ă����Ηǂ��B

	virtual LRESULT Blt(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		//	lpSrc��CDIB32*���A�b�v�L���X�g�������̂ł��邱�Ƃ͕ۏ؂���Ȃ�
		//	����Ė{���Ȃ��dynamic_cast<CDIB32*>(lpSrc)����NULL���`�F�b�N���ׂ�
		return Blt((CDIB32*)lpSrc,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}
	virtual LRESULT BltFast(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		return BltFast((CDIB32*)lpSrc,x,y,lpSrcRect,lpDstSize,lpClipRect);
	}
	virtual LRESULT BlendBlt(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		return BlendBlt((CDIB32*)lpSrc,x,y,dwDstRGBRate,dwSrcRGBRate,lpSrcRect,lpDstSize,lpClipRect);
	}
	virtual LRESULT BlendBltFast(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		return BlendBltFast((CDIB32*)lpSrc,x,y,dwDstRGBRate,dwSrcRGBRate,lpSrcRect,lpDstSize,lpClipRect);
	}

	virtual LRESULT FadeBltAlpha(CPlaneBase* lpSrc,int x,int y,int nFadeRate){
		//	���P�����̂��O�O�G ('01/02/28)
		DWORD dwRate = PlaneRGB(nFadeRate,nFadeRate,nFadeRate);
		return BlendBltFastAlpha(lpSrc,x,y,0xffffff-dwRate,dwRate);
	}

	virtual LRESULT ClearRect(LPRECT lpRect=NULL){
		return Clear(CLR_INVALID,lpRect);
	}
	virtual LRESULT LoadW(string szBitmapFileName256,string szBitmapFileNameElse
			,bool bLoadPalette=true) {
		return 1;	//	unsupported!!
	}

	//	���̒��o(��YGA�摜�̂Ƃ��͔����F�ȊO�Ȃ��255)
	virtual int	GetPixelAlpha(int x,int y){
		return m_lpDIB->GetPixelAlpha(x,y);
	}
};

#endif

#endif // ifdef USE_DIB32