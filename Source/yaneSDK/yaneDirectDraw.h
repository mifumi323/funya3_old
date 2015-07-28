// yaneDirectDraw.h
//	 This is DirectDraw wrapper.
//		programmed by yaneurao(M.Isozaki) '99/06/20
//		rewritten by yaneurao(M.Isozaki)  '00/07/08

#ifndef __yaneDirectDraw_h__
#define __yaneDirectDraw_h__

#include "yanePlaneBase.h"
#include "yanePlane.h"
#include "yaneDIB32.h"
#include "yaneWinHook.h"
#include "yaneLayer.h"

//////////////////////////////////////////////////////////////////////////////
//	surface bpp(bits per pixel manager)
//////////////////////////////////////////////////////////////////////////////

class CBppManager {
public:
	static int GetBpp();
	//	�����ł��擾�ł��܂��[�O�O�G

	static void Reset();
	//	����ʂ�bpp���ς�����Ƃ��́A���̊֐��Ăяo���ĂˁI

protected:
	static int m_nBpp;
};

//////////////////////////////////////////////////////////////////////////////
//	IDirectDraw wrapper
//////////////////////////////////////////////////////////////////////////////

#if defined(USE_DirectDraw) || defined(USE_FastDraw) || defined(USE_DIB32)

//	���̃N���X�́Asingleton�o�[�W����
class CDirectDrawBase {
public:
	static LPDIRECTDRAW GetDirectDrawPtr() { return m_lpDirectDraw; }

	CDirectDrawBase(bool bEmulationOnly=false);
	virtual ~CDirectDrawBase();

protected:
	static		LRESULT Initialize(bool bEmulationOnly=false);	//	DirectDraw interface�𓾂�
	static		LRESULT Terminate();	//	DirectDraw interface���������

	static		LPDIRECTDRAW	m_lpDirectDraw; // �������C���^�[�t�F�[�X
	static		int m_nRef;			//	�Q�ƃJ�E���^
};

//	���̃N���X�́A��singleton�o�[�W����
class CDirectDrawBaseM {
public:
	LPDIRECTDRAW GetDirectDrawPtr() { return m_lpDirectDraw; }

	CDirectDrawBaseM(bool bEmulationOnly=false);
	virtual ~CDirectDrawBaseM();

protected:
	LRESULT Initialize(bool bEmulationOnly=false);	//	DirectDraw interface�𓾂�
	LRESULT Terminate();	//	DirectDraw interface���������

	LPDIRECTDRAW	m_lpDirectDraw; // �������C���^�[�t�F�[�X
};


#endif

//////////////////////////////////////////////////////////////////
//	surface bpp manager
//////////////////////////////////////////////////////////////////
#if defined(USE_DirectDraw) || defined(USE_FastDraw)

//	�s�N�Z���t�H�[�}�b�g���ׂ�˂�I
class CDirectDrawSurfaceManager {
public:
	static int GetSurfaceType();
	//	�����ł��擾�ł��܂��[�O�O�G
	/*
		0:	�s��(������:OnChangeSurface���܂��Ăяo����Ă��Ȃ�)
		1:	�ȉ��̈ȊO
		2:	8bpp
		3:	16(RGB565)
		4:	16(RGB555)
		5:	24(RGB888)
		6:	24(BGR888)
		7:	32(XRGB8888)
		8:	32(XBGR8888)

		10:	16+a4(ARGB4565)	//	���ۂ́A���ꂪ�Ԃ��Ă��邱�Ƃ͖���
		11:	16+a4(ARGB4555)	//	���ۂ́A���ꂪ�Ԃ��Ă��邱�Ƃ͖���
		12:	32(ARGB8888)	//	���ۂ́A���ꂪ�Ԃ��Ă��邱�Ƃ͖���
		13:	32(ABGR8888)	//	���ۂ́A���ꂪ�Ԃ��Ă��邱�Ƃ͖���

	*/

	static void OnChangeSurface(LPDIRECTDRAWSURFACE);
	//	����ʂ�bpp���ς�����Ƃ��́A���̊֐��Ăяo���ĂˁI
	//	��������΁A���GetSurfaceType�ŁA
	//	���̃T�[�t�F�[�X�̒l���Ԃ�悤�ɂȂ�̂ŁI

	static int GetSurfaceType(LPDIRECTDRAWSURFACE);
	//	������T�[�t�F�[�X�̃T�[�t�F�[�X�^�C�v�𒲂ׂ邱�Ƃ��o����I

protected:
	static int m_nType;
};

#endif	//	#if defined(USE_DirectDraw) || defined(USE_FastDraw)

//////////////////////////////////////////////////////////////////
//	Window Clipper
//////////////////////////////////////////////////////////////////

#if defined(USE_DirectDraw) || defined(USE_FastDraw)

#ifdef USE_FastDraw
class CFastPlane;	//	Clip�ŕK�v�ƂȂ�̂őO���錾�B
#endif

class CWindowClipper {
public:
#ifdef USE_DirectDraw
	LRESULT Clip(CPlane*lpPrimary,HWND hWnd);	//	�������N���b�v����
#endif
#ifdef USE_FastDraw
	LRESULT Clip(CFastPlane*lpPrimary,HWND hWnd);	//	�������N���b�v����
#endif
	void	Release(void);						//	�N���b�p���������

	CWindowClipper(void);
	virtual ~CWindowClipper();

protected:
	LPDIRECTDRAWCLIPPER m_lpClipper;
};

#endif	//	#if defined(USE_DirectDraw) || defined(USE_FastDraw)

//////////////////////////////////////////////////////////////////////////////
//	CDirectDraw (DirectDrawWrapper)
//////////////////////////////////////////////////////////////////////////////
#ifdef	USE_DirectDraw

class CDirectDraw : public CWinHook,public CPlaneBase {
public:

	//////////////////////////////////////////
	//	�f�B�X�v���C���[�h�̕ύX

	LRESULT		SetDisplay(bool bFullScr=false,int nSizeX=0,int nSizeY=0,int nColorDepth=0);
	void		GetDisplay(bool&bFullScr,int &nSizeX,int &nSizeY,int &nColorDepth);
	bool		IsFullScreen(void);
	int			GetBpp(void);	// ���݂�Bpp�̎擾

	//	Begin�`End�Ńf�B�X�v���C���[�h��ύX����B
	void		BeginChangeDisplay(void);
	void		TestDisplayMode(int nSX,int nSY,bool bFullScr=false,int nColor=0);
	LRESULT		EndChangeDisplay(void);

	//	�t���b�v�͎g���̂��H
	void		SetFlipUse(bool);	// fullscreen��DirectDraw��Flip���g�����H
	bool		GetFlipUse(void);	// ���ʂ̎擾
	void		FlipToGDISurface(void); //	GDISurface�Ƀt���b�v����

	//////////////////////////////////////////
	//	�v���C�}���ƃZ�J���_���̎擾

	CPlane*		GetPrimary(void)		{ m_bDirty = true; return &m_Primary; }
	CPlane*		GetSecondary(void)		{ m_bDirty = true; return &m_Secondary; }

	//	�����t���O�𗧂Ă�
	void		Invalidate(void) { m_bDirty = true; }

#ifdef USE_DIB32
	LRESULT		CreateSecondaryDIB(void);
	void		ReleaseSecondaryDIB(void);
	CDIB32*		GetSecondaryDIB(void)	{ return m_lpSecondaryDIB; }
#endif

	//	�T�[�t�F�[�X�̃��X�g�`�F�b�N
	void		CheckSurfaceLost(void);

	///////////////////////////////////////////////////////////////////////////
	//	Secondary�v���[���ւ̓]���n(��{�I��CPlane�ɈϏ�����)
	
	LRESULT		Blt(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPRECT lpClipDstRect=NULL);
	LRESULT		BltFast(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPRECT lpClipDstRect=NULL);
	LRESULT		BlendBlt(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect=NULL,LPRECT lpClipDstRect=NULL);
	LRESULT		BlendBltFast(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect=NULL,LPRECT lpClipDstRect=NULL);

	LRESULT		BltR(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipDstRect=NULL);
	LRESULT		BltFastR(CPlane*lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipDstRect=NULL);
	LRESULT		BlendBltR(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipDstRect=NULL);
	LRESULT		BlendBltFastR(CPlane*lpSrc,int x,int y,int ar,int ag,int ab,int br,int bg,int bb,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipDstRect=NULL);

	//	override from CPlaneBase
	virtual LRESULT BlendBltFastAlpha(CPlaneBase* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return -1; // not supported
	}
	virtual LRESULT BlendBltFastAlpha(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
		,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {;
		return -1; // not supported
	}
	virtual LRESULT FadeBltAlpha(CPlaneBase* lpSrc,int x,int y,int nFadeRate){
		return -1; // not supported
	}
	virtual bool IsLoaded(void) const;

#ifdef USE_DIB32
	LRESULT		Blt(CDIB32*lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPRECT lpClipRect=NULL);
#endif

	//////////////////////////////////////////
	//	Secondary�v���[���̕`��n(Secondary�v���[���ɈϏ�����)
	
	LRESULT		Clear(LPRECT lpRect=NULL);
	LRESULT		SetFillColor(COLORREF c);		//	Clear����F���w�肷��(Default==RGB(0,0,0))
	DWORD		GetFillColor(void);				//	FillColor����F

	///////////////////////////////////////////////////////////////////////////
	//	SecondaryDIB�ւ̓]���n(��{�I��CDIB32�ɈϏ�����)
	
	//		�֐��p�ӂ���̖ʓ|������AGetSecondaryDIB()�������ĂˁO�O

	//////////////////////////////////////////
	//	Secondary->Primary�v���[���̓]��
	
	virtual void	OnDraw(RECT* lpRect=NULL,bool bLayerCallBack=true);//	Secondary->Primary�ւ̓]��
#ifdef USE_DIB32
	virtual void	OnDrawDIB(RECT* lpRect=NULL,bool bLayerCallBack=true);//	SecondaryDIB->Primary�ւ̓]��
#endif
	void	SetOffset(int ox,int oy);	//	�Z�J���_���̓]���I�t�Z�b�g
	void	SetBrightness(int nBright); //	�t�F�[�h

	//////////////////////////////////////////
	//	���C���̊Ǘ�

	//	Layer�̃��X�g��Ԃ�
	CLayerList* GetLayerList(void) { return &m_LayerList; }
	CLayerList* GetAfterLayerList(void) { return &m_AfterLayerList; }
	CLayerList* GetHDCLayerList(void) { return &m_HDCLayerList; }

	//////////////////////////////////////////
	//	CPlaneBase�Ƃ��ĐU�������߂̊֐�(���ׂ�Secondary�ֈϏ�����)

	//	CPlaneBase�̊֐��̎�����ۏ�
	virtual LRESULT Blt(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BltFast(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBlt(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
	virtual LRESULT BlendBltFast(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRat
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);

	virtual LRESULT MosaicEffect(int d, LPRECT lpRect=NULL);
	virtual LRESULT FlushEffect(LPRECT lpRect=NULL);
	virtual void	GetSize(int &x,int &y);
	virtual LRESULT ClearRect(LPRECT lpRect=NULL);
	virtual LRESULT Load(string szBitmapFileName,bool bLoadPalette=false);
	virtual LRESULT LoadW(string szBitmapFileName256,string szBitmapFileNameElse
			,bool bLoadPalette=true);
	virtual LRESULT Release(void);
	virtual LRESULT SetColorKey(int x,int y);
	virtual LRESULT SetColorKey(int r,int g,int b);

	//////////////////////////////////////////
	//	���܂�

	//	�����DirectDrawPtr���擾����
	LPDIRECTDRAW	GetDDraw() { return m_DirectDrawBase.GetDirectDrawPtr(); }

	//////////////////////////////////////////

	CDirectDraw(void);
	virtual ~CDirectDraw();

protected:
	//	�}���`�E�B���h�D�Ή��łƍ����ւ�
	CDirectDrawBaseM m_DirectDrawBase;

	//	�����DirectDrawPtr���擾����
//	LPDIRECTDRAW	GetDDraw(void) { return m_DirectDrawBase.GetDirectDrawPtr(); }

	//	��ʃ��[�h�̕ύX�֘A
	LRESULT		ChangeDisplay(bool bFullScr);	//	���݂̃f�B�X�v���C���[�h�𔽉f������
	bool	m_bFullScr;			//	�t���X�N���[�����[�h���H
	int		m_nScreenXSize;		//	��ʃT�C�Y
	int		m_nScreenYSize;
	int		m_nScreenColor;		//	���bpp
	bool	m_bDisplayChanging; //	�𑜓x�ύX��
	bool	m_bChangeDisplayMode;// �f�B�X�v���C���[�h��ύX���邩 // �ǉ�

	//	�v���C�}���T�[�t�F�[�X�ƃZ�J���_���T�[�t�F�[�X
	CPlane	m_Primary;
	CPlane	m_Secondary;
	bool	m_bUseFlip;			//	�t���X�N���[�����Ƀt���b�v���g���̂�(���[�U�[�̊�])
	bool	m_bUseFlip2;		//	�t���b�v���g�����[�h�ɂȂ��Ă���̂�(����)
	CWindowClipper m_WindowClipper;
	int		m_nSecondaryOffsetX;		//	�Z�J���_���̓]���I�t�Z�b�g��
	int		m_nSecondaryOffsetY;

#ifdef USE_DIB32
	//	�Z�J���_��DIB32�T�[�t�F�[�X
	CDIB32* m_lpSecondaryDIB;
#endif

	bool	m_bDirty;			//	�Z�J���_���v���[���͑O��v���C�}���ɓ]�������Ƃ����牘�ꂽ���H
	HWND	m_hWnd;				//	��������CAppInitializer����擾����ƒx���Ȃ�̂�

	//	for Layer management
	CLayerList	m_LayerList;
	CLayerList	m_AfterLayerList;
	CLayerList	m_HDCLayerList;

	//	��ʂ̃t�F�[�h�֘A(OnDraw�ŗ��p�����)
	int		m_nBrightness;
	void	RealizeBrightness(int nBrightness);

	//	�T�[�t�F�[�X�̃��X�g�`�F�b�N�i�����p�j
	bool	m_bLostSurface;

	// override from CWinHook
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};
#endif //  #if defined(USE_DirectDraw) || defined(USE_FastDraw)

#endif // __yaneDirectDraw_h__