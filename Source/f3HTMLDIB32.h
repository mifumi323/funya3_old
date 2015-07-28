// BWHTMLDIB32.h: CBWHTMLDIB32 クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

// BWHTMLDIB32の08/09/17版を基に作成

#ifndef __F3HTMLDIB32_H__
#define __F3HTMLDIB32_H__

#include "YANESDK\yaneDIB32.h"

// 書式と文字列のセット
struct Tf3HTMLPiece
{
	string		data;			// 表示するテキストまたは画像ファイル名
	RECT		rcClip;			// クリッピング範囲
	SIZE		szImage;		// イメージのサイズ

	int			fontSize;		// フォントサイズ
	COLORREF	rgbColor;		// 文字色

	bool		bBold;			// 太字
	bool		bItalic;		// イタリック体
	bool		bUnderLine;		// アンダーライン
	bool		bStrikeOut;		// 取り消し線

	bool		bImage;			// イメージかどうか
	bool		bBreak;			// 改行かどうか
};

enum HTMLAlign { AlignLeft, AlignCenter, AlignRight };
enum HTMLVAlign { AlignBottom, AlignMiddle, AlignTop };

class Cf3HTMLDIB32 : public CDIB32  
{
	friend class Parser;
	friend class Renderer;
public:
	void SetVAlign(HTMLVAlign valign) { m_DefaultVAlign = valign; }
	HTMLVAlign GetVAlign() { return m_DefaultVAlign; }
	void SetAlign(HTMLAlign align) { m_DefaultAlign = align; }
	HTMLAlign GetAlign() { return m_DefaultAlign; }
	void OnDraw(CDIB32* lp, int x, int y, int length=INT_MAX);
	COLORREF GetColorByName(LPCSTR strName);
	void SetMaxWidth(int w);
	int GetFontSize(int relative);
	void SetFontSize(int nSize);
	void UpdateCondition();
	void Update();
	void __cdecl SetText(LPSTR fmt, ... );
	void SetText(const string &s);
	Cf3HTMLDIB32();
	virtual ~Cf3HTMLDIB32();

protected:
	void InnerBlt(CDIB32*lp, int x, int y, LPRECT rc=NULL);
	bool m_bDirty;				// 再描画が必要か

	string m_String;				// HTMLタグを含む文字列
	string m_PureText;				// タグを含まない文字列
	list<Tf3HTMLPiece> m_Pieces;	// 書式と文字列のセット
	vector<RECT> m_Rects;			// 一文字ごとの描画エリア

	int m_nFontSize;
	int m_nMaxWidth;
	int m_nLength;

	HTMLAlign m_Align, m_DefaultAlign;
	HTMLVAlign m_VAlign, m_DefaultVAlign;
};

#endif
