// yaneDIBDraw.h
//   This is DIBDraw wrapper.
//      programmed by yaneurao(M.Isozaki) '00/10/02

#ifdef USE_DIB32

#ifndef __yaneDIBDraw_h__
#define __yaneDIBDraw_h__

#include "yaneDirectDraw.h"
#include "yanePlaneBase.h"

class CDIB32;

//////////////////////////////////////////////////////////////////////////////

//  DIB32�����Ƃ���`��N���X
class CDIBDraw : public CWinHook,public CPlaneBase {
public:

    //////////////////////////////////////////
    //  �f�B�X�v���C���[�h�̕ύX

    LRESULT     SetDisplay(bool bFullScr=false,int nSizeX=0,int nSizeY=0,int nColorDepth=0);
    void        GetDisplay(bool&bFullScr,int &nSizeX,int &nSizeY,int &nColorDepth);
    bool        IsFullScreen(void);
    int         GetBpp(void);   // ���݂�Bpp�̎擾

    //  Begin�`End�Ńf�B�X�v���C���[�h��ύX����B
    void        BeginChangeDisplay(void);
    void        TestDisplayMode(int nSX,int nSY,bool bFullScr=false,int nColor=0);
    LRESULT     EndChangeDisplay(void);

    //////////////////////////////////////////
    //  �v���C�}���ƃZ�J���_���̎擾

    CDIB32*     GetSecondary(void)  { m_bDirty = true; return m_lpSecondary; }

    //  �����t���O�𗧂Ă�
    void        Invalidate(void) { m_bDirty = true; }

    ///////////////////////////////////////////////////////////////////////////
    //  Secondary�v���[���ւ̓]���n(���ׂ�CDIB�ɈϏ�����)

    //  CPlaneBase�̊֐��̎�����ۏ�
    virtual LRESULT Blt(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
    virtual LRESULT BltFast(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
    virtual LRESULT BlendBlt(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
                ,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
    virtual LRESULT BlendBltFast(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRat
                ,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
    virtual LRESULT BltNatural(CPlaneBase* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
    virtual LRESULT BltNatural(CPlaneBase* lpDIBSrc32,int x,int y,int nFade,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);

    virtual LRESULT MosaicEffect(int d, LPRECT lpRect=NULL);
    virtual LRESULT FlushEffect(LPRECT lpRect=NULL);
    virtual void    GetSize(int &x,int &y);
    virtual LRESULT ClearRect(LPRECT lpRect=NULL);

    virtual LRESULT Load(string szBitmapFileName,bool bLoadPalette=false);
    virtual LRESULT LoadW(string szBitmapFileName256,string szBitmapFileNameElse
            ,bool bLoadPalette=true);
    virtual LRESULT Release(void);
    virtual LRESULT SetColorKey(int x,int y);
    virtual LRESULT SetColorKey(int r,int g,int b);
    virtual bool IsLoaded(void) const;

    //////////////////////////////////////////
    //  Secondary�v���[���̕`��n(Secondary�v���[���ɈϏ�����)
    
    LRESULT Clear(DWORD dwDIB32RGB=0,LPRECT lpRect=NULL);

    LRESULT BltClearAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
    LRESULT BltFastClearAlpha(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
    //  override from CPlaneBase
    virtual LRESULT BlendBltFastAlpha(CPlaneBase* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
    virtual LRESULT BlendBltFastAlpha(CPlaneBase* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
                ,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
    virtual LRESULT FadeBltAlpha(CPlaneBase* lpSrc,int x,int y,int nFadeRate);

    LRESULT BltClearAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
    LRESULT BltFastClearAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
    LRESULT BlendBltFastAlphaM(CDIB32* lpDIBSrc32,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);
    LRESULT BlendBltFastAlphaM(CDIB32* lpDIBSrc32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRat
                ,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL);

    //////////////////////////////////////////
    //  Secondary->Primary�v���[���̓]��
    
    virtual void    OnDraw(RECT* lpRect=NULL,bool bLayerCallBack=true);     //  Secondary->Primary�ւ̓]��
                                            //  bLayerCallBack=false�̎��̓��C���[���Ă΂Ȃ�
    void    SetOffset(int ox,int oy);   //  �Z�J���_���̓]���I�t�Z�b�g
    void    SetBrightness(int nBright); //  �t�F�[�h

    //////////////////////////////////////////
    //  ���C���̊Ǘ�

    //  Layer�̃��X�g��Ԃ�
    CLayerList* GetLayerList(void) { return &m_LayerList; }
    CLayerList* GetHDCLayerList(void) { return &m_HDCLayerList; }
    CLayerList* GetAfterLayerList(void) { return &m_AfterLayerList; }

    //////////////////////////////////////////
    //  property..

    int     GetMenuHeight(void);
    //  ���j���[�����Ă���Ȃ�΁A���̃��j���[������Ԃ�

    //////////////////////////////////////////

    CDIBDraw(void);
    virtual ~CDIBDraw();

protected:
    CDirectDrawBase m_DirectDrawBase;

    //  �����DirectDrawPtr���擾����
    LPDIRECTDRAW    GetDDraw(void) { return m_DirectDrawBase.GetDirectDrawPtr(); }

    //  ��ʃ��[�h�̕ύX�֘A
    LRESULT     ChangeDisplay(bool bFullScr);   //  ���݂̃f�B�X�v���C���[�h�𔽉f������
    bool    m_bFullScr;         //  �t���X�N���[�����[�h���H
    int     m_nScreenXSize;     //  ��ʃT�C�Y
    int     m_nScreenYSize;
    int     m_nScreenColor;     //  ���bpp
    bool    m_bDisplayChanging; //  �𑜓x�ύX��
	bool	m_bChangeDisplayMode;// �f�B�X�v���C���[�h��ύX���邩 // �ǉ�
    int     m_nMenu;            //  ���j���[����

    //  �Z�J���_��DIB32�T�[�t�F�[�X
    auto_ptrEx<CDIB32>  m_lpSecondary;
    int     m_nSecondaryOffsetX;        //  �Z�J���_���̓]���I�t�Z�b�g��
    int     m_nSecondaryOffsetY;

    bool    m_bDirty;           //  �Z�J���_���v���[���͑O��v���C�}���ɓ]�������Ƃ����牘�ꂽ���H
    HWND    m_hWnd;             //  ��������CAppInitializer����擾����ƒx���Ȃ�̂�

    //  for Layer management
    CLayerList  m_LayerList;
    CLayerList  m_AfterLayerList;
    CLayerList  m_HDCLayerList;

    //  ��ʂ̃t�F�[�h�֘A(OnDraw�ŗ��p�����)
    int     m_nBrightness;
    void    RealizeBrightness(int nBrightness);
    //  �����I�ȕ`��
    void    InnerOnDraw(HDC hDst,HDC hSrc,RECT* lpRect=NULL);

    // override from CWinHook
    LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

#endif

#endif // ifdef USE_DIB32