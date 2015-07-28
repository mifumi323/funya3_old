//	DirectDrawSurface wrapper

#ifdef USE_DirectDraw

#ifndef __yanePlane_h__
#define __yanePlane_h__

#include "yanePlaneBase.h"

class CDirectDraw;
#ifdef USE_DIB32
class CDIB32;
#endif

class CPlane : public CPlaneBase {
public:

	//////////////////////////////////////////////////////////////////////////
	//	CPlaneBase�̔h���N���X�ł́A������I�[�o�[���C�h���ׂ�
	virtual EDrawType GetID() const { return eDraw_CPlane; }

	//////////////////////////////////////////////////////////////////////////
	// �r�b�g�}�b�v�֘A
	virtual LRESULT Load(string szBitmapFileName,bool bLoadPalette=true);
	// bLoadPalette==false���ƁA���݂̃p���b�g�J���[�ɏ�����SetDIBitsToDevice��
	// �ǂݍ��܂��BWM_PALETTECHANGED�ɉ�������A�v���̏ꍇ�A����œǂݍ��ޕK�v����

	virtual LRESULT LoadW(string szBitmapFileName256,string szBitmapFileNameElse
			,bool bLoadPalette=true);
	// 256�F���[�h�Ȃ�΁A�ʂ̃t�@�C����ǂޏꍇ

	//	�r�b�g�}�b�v�t�@�C���Ƃ��ĕۑ�����
	LRESULT	Save(LPSTR szFileName,LPRECT lpRect=NULL);

	virtual void	GetSize(int &x,int &y);		// �������ꂽSurface�̃T�C�Y�𓾂�
	virtual LRESULT	Release(void);

	//////////////////////////////////////////////////////////////////////////
	// ���߃L�[�ݒ�֘A
	LRESULT	SetColorKey(COLORREF rgb);	// ����̐F��]���̂Ƃ��̓��߃L�[�ɐݒ肷��
	LRESULT SetColorKey(int r,int g,int b);
	static DWORD DDColorMatch(LPDIRECTDRAWSURFACE pdds, COLORREF rgb); // �����F��T��

	virtual LRESULT SetColorKey(int x,int y);	// (x,y)�̓_�𓧉߃L�[�ɐݒ肷��
	static DWORD DDGetPixel(LPDIRECTDRAWSURFACE pdds,int x,int y); // ����̓_�̐F�𒲂ׂ�

	//////////////////////////////////////////////////////////////////////////
	//	�ʏ�]���n

	LRESULT		Blt(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPRECT lpClipDstRect=NULL);
	LRESULT		BltFast(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPRECT lpClipDstRect=NULL);
	LRESULT		BlendBlt(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect=NULL,LPRECT lpClipDstRect=NULL);
	LRESULT		BlendBltFast(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect=NULL,LPRECT lpClipDstRect=NULL);

	//	�g�k����
	LRESULT		BltR(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipDstRect=NULL);
	LRESULT		BltFastR(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipDstRect=NULL);
	LRESULT		BlendBltR(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipDstRect=NULL);
	LRESULT		BlendBltFastR(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipDstRect=NULL);

#ifdef USE_DIB32
	//	CDIB32�Ƃ̂��Ƃ�
	LRESULT		Blt(CDIB32*lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPRECT lpClipRect=NULL);
	LRESULT		BltTo(CDIB32*lpDst,int x,int y,LPRECT lpSrcRect=NULL,LPRECT lpClipRect=NULL);
#endif
	//////////////////////////////////////////////////////////////////////////
	//	�ʏ�G�t�F�N�g�n

	LRESULT		Clear(LPRECT lpRect=NULL);		//	��`�N���A
	LRESULT		SetFillColor(COLORREF c);		//	Clear����F���w�肷��(Default==RGB(0,0,0))
	DWORD		GetFillColor(void);				//	FillColor����F

	//	Mosaic�i���̃v���[���ɑ΂���G�t�F�N�g�j
	virtual LRESULT MosaicEffect(int d, LPRECT lpRect=NULL);
	//	Flush �i���̃v���[���ɑ΂���G�t�F�N�g�j
	virtual LRESULT FlushEffect(LPRECT lpRect=NULL);

	//////////////////////////////////////////////////////////////////////////
	//	�e��ݒ�
	LRESULT	SetSystemMemoryUse(bool bEnable);	//	�V�X�e�����������g���̂�

	//////////////////////////////////////////////////////////////////////////
	//	HDC�𓾂āA���ڏ�������:p
	HDC		GetDC(void);
	void	ReleaseDC(void);

	//	�I�[�i�[�h���[�T�[�t�F�[�X�̐���
	virtual LRESULT CreateSurface(int sx,int sy,bool bYGA=false);		//	�T�C�Y�w��Ńv���[���쐬

	//	owner drawing bitmap(�����v���[��)�t���O�擾
	bool*	GetHybrid(void) { return& m_bHybrid; }

	//	�v���C�}���T�[�t�F�[�X�̐���
	LRESULT	CreatePrimary(bool& bUseFilp,int nSx=0,int nSy=0);
	//	�Z�J���_���T�[�t�F�[�X�̐���
	LRESULT CreateSecondary(CPlane*lpPrimary,bool& bUseFlip);

	//�@���v���[���Ɠ��ꊷ����@�\
	void	SwapPlane(CPlane*lpPlane);

	//////////////////////////////////////////////////////////////////////////
	//	property...

	//	�r�b�g�}�b�v���ǂݍ��܂�Ă����true
	bool	IsLoaded(void) const { return m_bBitmap; }
	//	�t�@�C������Ԃ�
	string	GetFileName(void) const;

	LPDIRECTDRAWSURFACE GetSurface(void);
	LPDIRECTDRAWPALETTE GetPalette(void);

	//////////////////////////////////////////////////////////////////////////
	//	static members..
	static	int	GetBpp(void);			//	���݂�bpp���擾

	// WM_ACTIVATEAPP���ɃT�[�t�F�[�X�̃��X�g�A���s�Ȃ����߂̂���
	static LRESULT RestoreAll(void);		//	�S�v���[���̃��X�g�A

	CPlane(void);
	virtual ~CPlane();

	friend class CDirectDraw;

protected:
	LPDIRECTDRAWSURFACE m_lpSurface;
	LPDIRECTDRAWPALETTE m_lpPalette;
	int		m_nSurfaceRef;	//	�T�[�t�F�[�X�̎Q�ƃJ�E���^
							//	DirectDraw��Duplicate�̓o�O�L��Ȃ̂Ŏ��O�ł��O�O

	//	�v���[���̃T�C�Y
	int		m_nSizeX;
	int		m_nSizeY;

	//	bitmap�t�@�C����
	string	m_szBitmapFile;				// �t�@�C�����ۑ����Ƃ��Ȃ�������
	string	m_szBitmapFile2;			// LoadBitmapFileW�ł̑��F���[�h�̃t�@�C����
	bool	m_bBitmapW;					// m_DirectDrawBitmap2�͗L���Ȃ̂��H
	bool	m_bLoadPalette;

	//	bitmap��ǂݍ���Ă���̂��A����Ƃ�
	//	CreateSurface�ɂ���č쐬���ꂽ�v���[�����H
	bool	m_bBitmap;
	virtual LRESULT OnDraw(void);		//	CreateSurface�ɂ���č쐬�����ꍇ�A
										//	�v���[���̏����Ɋւ��Ă��ꂪ�Ăяo�����

	//	�����I�Ɏg�p(�h���N���X�ŕK�v�ł����override���Ă�)
	virtual void	ResetColorKey(void);		//	�J���[�L�[�̃��Z�b�g
	virtual LRESULT	InnerLoad(string szFileName,bool bLoadPalette);
	virtual LRESULT InnerCreateSurface(int sx,int sy);	//	�T�C�Y�w��Ńv���[���쐬

	bool	m_bOwnerDraw;				//	�ǂݍ���ł���̂̓r�b�g�}�b�v�ł͂Ȃ��I�[�i�[�h���[�^�Ȃ̂��H
	LRESULT Restore(void);				//	�T�[�t�F�[�X�̃��X�g�A

	//////////////////////////////////////////////////////////////////////////

	// ���߃L�[�֘A
	bool	m_bUsePosColorKey;			// �ʒu�w��^��ColorKey��(true)�A�F�w��^���H(false)
	COLORREF m_ColorKey;				// �F�w��^�@���߃J���[�L�[
	int		m_nCX,m_nCY;				// �ʒu�w��^�@���߃J���[�L�[
	HDC		m_hDC;						// BeginPaint�`EndPaint�p
	LRESULT	SetColorKey(void);			//	���A�p�i�����I�Ɏg�p�j
	bool	m_bUseSystemMemory;			//	�����I�ɃV�X�e�����������g���I�v�V����

	COLORREF m_FillColor;				//	��`�N���A����F
	DWORD	m_dwFillColor;				//	���̐F�����݂̉�ʃ��[�h�ɍ��킹�ĕϊ���������

	bool	m_bHybrid;					//	owner drawing bitmap���H
	bool	CheckDuplicate(string szFileName);	//	Duplicate�\���H
	void	ReleaseDDSurface(void);		//	�Q�ƃJ�E���g���̃����[�X
	void	ClearMSB(void);				//	32bpp���[�h�ōŏ�ʃo�C�g�ɏ��グ���S�~������

	//////////////////////////////////////////////////////////////////////////

	//	�S�v���[���ւ̃C���X�^���X�̃`�F�C��
	//	���̃`�F�C���ɂ�Primary,Secondary���܂ނ̂Œ��ӂ��K�v
	static set<CPlane*>	m_lpPlaneList;

	//////////////////////////////////////////////////////////////////////////

	static void	InnerGetBpp(void);	//	���݂�bpp���擾

	//////////////////////////////////////////////////////////////////////////
	//	override from CPlaneBase
	//	�����́A���ڌĂяo����邱�Ƃ͖����̂�protected�ɂ��Ă����Ηǂ��B

	virtual LRESULT Blt(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		if (lpDstSize==NULL) {
		//	lpSrc��CPlane*���A�b�v�L���X�g�������̂ł��邱�Ƃ͕ۏ؂���Ȃ�
		//	����Ė{���Ȃ��dynamic_cast<CPlane*>(lpSrc)����NULL���`�F�b�N���ׂ�
			return Blt((CPlane*)lpSrc,x,y,lpSrcRect,lpClipRect);
		} else {
			return BltR((CPlane*)lpSrc,x,y,lpSrcRect,lpDstSize,lpClipRect);
		}
	}
	virtual LRESULT BltFast(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		if (lpDstSize==NULL) {
			return BltFast((CPlane*)lpSrc,x,y,lpSrcRect,lpClipRect);
		} else {
			return BltFastR((CPlane*)lpSrc,x,y,lpSrcRect,lpDstSize,lpClipRect);
		}
	}
	virtual LRESULT BlendBlt(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		if (lpDstSize==NULL) {
			return BlendBlt((CPlane*)lpSrc,x,y
				,dwSrcRGBRate>>16,(dwSrcRGBRate>>8)&0xff,dwSrcRGBRate & 0xff
				,dwDstRGBRate>>16,(dwDstRGBRate>>8)&0xff,dwDstRGBRate & 0xff
				,lpSrcRect,lpClipRect);
		} else {
			return BlendBltR((CPlane*)lpSrc,x,y
				,dwSrcRGBRate>>16,(dwSrcRGBRate>>8)&0xff,dwSrcRGBRate & 0xff
				,dwDstRGBRate>>16,(dwDstRGBRate>>8)&0xff,dwDstRGBRate & 0xff
				,lpSrcRect,lpDstSize,lpClipRect);
		}
	}
	virtual LRESULT BlendBltFast(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		if (lpDstSize==NULL) {
			return BlendBltFast((CPlane*)lpSrc,x,y
				,dwSrcRGBRate>>16,(dwSrcRGBRate>>8)&0xff,dwSrcRGBRate & 0xff
				,dwDstRGBRate>>16,(dwDstRGBRate>>8)&0xff,dwDstRGBRate & 0xff
				,lpSrcRect,lpClipRect);
		} else {
			return BlendBltFastR((CPlane*)lpSrc,x,y
				,dwSrcRGBRate>>16,(dwSrcRGBRate>>8)&0xff,dwSrcRGBRate & 0xff
				,dwDstRGBRate>>16,(dwDstRGBRate>>8)&0xff,dwDstRGBRate & 0xff
				,lpSrcRect,lpDstSize,lpClipRect);
		}
	}
	virtual LRESULT BlendBltFastAlpha(CPlaneBase* lpSrc,int x,int y
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		return -1; // ������
	}
	virtual LRESULT BlendBltFastAlpha(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		return -1; // ������
	}
	virtual LRESULT FadeBltAlpha(CPlaneBase* lpSrc,int x,int y,int nFadeRate){
		return -1; // ������
	}
	virtual LRESULT	ClearRect(LPRECT lpRect=NULL){
		return Clear(lpRect);
	}


};

#endif

#endif // USE_DirectDraw