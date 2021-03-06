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

//  DIB32を基底とする描画クラス
class CDIBDraw : public CWinHook,public CPlaneBase {
public:

    //////////////////////////////////////////
    //  ディスプレイモードの変更

    LRESULT     SetDisplay(bool bFullScr=false,int nSizeX=0,int nSizeY=0,int nColorDepth=0);
    void        GetDisplay(bool&bFullScr,int &nSizeX,int &nSizeY,int &nColorDepth);
    bool        IsFullScreen(void);
    int         GetBpp(void);   // 現在のBppの取得

    //  Begin〜Endでディスプレイモードを変更する。
    void        BeginChangeDisplay(void);
    void        TestDisplayMode(int nSX,int nSY,bool bFullScr=false,int nColor=0);
    LRESULT     EndChangeDisplay(void);

    //////////////////////////////////////////
    //  プライマリとセカンダリの取得

    CDIB32*     GetSecondary(void)  { m_bDirty = true; return m_lpSecondary; }

    //  汚しフラグを立てる
    void        Invalidate(void) { m_bDirty = true; }

    ///////////////////////////////////////////////////////////////////////////
    //  Secondaryプレーンへの転送系(すべてCDIBに委譲する)

    //  CPlaneBaseの関数の実装を保証
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
    //  Secondaryプレーンの描画系(Secondaryプレーンに委譲する)
    
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
    //  Secondary->Primaryプレーンの転送
    
    virtual void    OnDraw(RECT* lpRect=NULL,bool bLayerCallBack=true);     //  Secondary->Primaryへの転送
                                            //  bLayerCallBack=falseの時はレイヤーを呼ばない
    void    SetOffset(int ox,int oy);   //  セカンダリの転送オフセット
    void    SetBrightness(int nBright); //  フェード

    //////////////////////////////////////////
    //  レイヤの管理

    //  Layerのリストを返す
    CLayerList* GetLayerList(void) { return &m_LayerList; }
    CLayerList* GetHDCLayerList(void) { return &m_HDCLayerList; }
    CLayerList* GetAfterLayerList(void) { return &m_AfterLayerList; }

    //////////////////////////////////////////
    //  property..

    int     GetMenuHeight(void);
    //  メニューがついているならば、そのメニュー高さを返す

    //////////////////////////////////////////

    CDIBDraw(void);
    virtual ~CDIBDraw();

protected:
    CDirectDrawBase m_DirectDrawBase;

    //  これでDirectDrawPtrを取得する
    LPDIRECTDRAW    GetDDraw(void) { return m_DirectDrawBase.GetDirectDrawPtr(); }

    //  画面モードの変更関連
    LRESULT     ChangeDisplay(bool bFullScr);   //  現在のディスプレイモードを反映させる
    bool    m_bFullScr;         //  フルスクリーンモードか？
    int     m_nScreenXSize;     //  画面サイズ
    int     m_nScreenYSize;
    int     m_nScreenColor;     //  画面bpp
    bool    m_bDisplayChanging; //  解像度変更中
	bool	m_bChangeDisplayMode;// ディスプレイモードを変更するか // 追加
    int     m_nMenu;            //  メニュー高さ

    //  セカンダリDIB32サーフェース
    auto_ptrEx<CDIB32>  m_lpSecondary;
    int     m_nSecondaryOffsetX;        //  セカンダリの転送オフセット量
    int     m_nSecondaryOffsetY;

    bool    m_bDirty;           //  セカンダリプレーンは前回プライマリに転送したときから汚れたか？
    HWND    m_hWnd;             //  いちいちCAppInitializerから取得すると遅くなるので

    //  for Layer management
    CLayerList  m_LayerList;
    CLayerList  m_AfterLayerList;
    CLayerList  m_HDCLayerList;

    //  画面のフェード関連(OnDrawで利用される)
    int     m_nBrightness;
    void    RealizeBrightness(int nBrightness);
    //  内部的な描画
    void    InnerOnDraw(HDC hDst,HDC hSrc,RECT* lpRect=NULL);

    // override from CWinHook
    LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

#endif

#endif // ifdef USE_DIB32