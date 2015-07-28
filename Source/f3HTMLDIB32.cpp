// f3HTMLDIB32.cpp: Cf3HTMLDIB32 クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3HTMLDIB32.h"
#include  <mbctype.h> // _ismbblead
//#include "BWDraw.h"
//#include "DIBResource.h"
#include "ResourceManager.h"

inline int ctoi(char c) {
	if ('0'<=c&&c<='9') return c-'0';
	if ('A'<=c&&c<='F') return c-'A'+10;
	if ('a'<=c&&c<='f') return c-'a'+10;
	return 0;
}

class Parser {
private:
	enum TagType {	// 後ろに「_」をつけた定数は閉じタグにしよう。そうしよう
		TagB,		TagB_,		// 太字。画面を拡大したとき困りそうだ
		TagBIG,					// 大きくする。閉じタグはSMALLと同じ
		TagBR,					// いわずと知れた改行
		TagDIV,					// 寄せる位置を決める。閉じタグはBRと同じ
		TagFONT,	TagFONT_,	// COLORとSIZEを変更できる
		TagI,		TagI_,		// 斜体。これまた予想できないタグ
		TagIMG,					// 画像表示は必須だろう
		TagS,		TagS_,		// 論理タグより物理タグだよな
		TagSMALL,				// BIGの逆
		TagU,		TagU_,		// 強調にはこっちのほうがいいかもね
		TagUNKNOWN,				// まあ、その他大勢とも言う。何もしない
	};

	Cf3HTMLDIB32*m_lpHTML;
	int m_nSeek;

	list<Tf3HTMLPiece>& Pieces() { return m_lpHTML->m_Pieces; }
	Tf3HTMLPiece& Current() { return Pieces().back(); }
	int& Length() { return m_lpHTML->m_nLength; }
	const string& String() { return m_lpHTML->m_String; }

	char Read() { return m_nSeek<String().length()?String().c_str()[m_nSeek++]:'\0'; }
	void Unread() { m_nSeek--; }
	COLORREF GetColor(LPCSTR strColor) {
		if (*strColor=='#') {
			switch (strlen(strColor+1)) {
			case 6: return 
					(ctoi(strColor[5])<<24) |
					(ctoi(strColor[6])<<16) |
					(ctoi(strColor[3])<<12) |
					(ctoi(strColor[4])<<8) |
					(ctoi(strColor[1])<<4) |
					ctoi(strColor[2]);
			case 3: return 
					(ctoi(strColor[3])*17<<16) |
					(ctoi(strColor[2])*17<<8) |
					ctoi(strColor[1])*17;
			}
		}else  {
			if (stricmp(strColor, "white")==0) return 0xffffff;
			if (stricmp(strColor, "snow")==0) return 0xfafaff;
			if (stricmp(strColor, "ghostwhite")==0) return 0xfff8f8;
			if (stricmp(strColor, "ivory")==0) return 0xf0ffff;
			if (stricmp(strColor, "mintcream")==0) return 0xfafff5;
			if (stricmp(strColor, "azure")==0) return 0xfffff0;
			if (stricmp(strColor, "floralwhite")==0) return 0xf0faff;
			if (stricmp(strColor, "aliceblue")==0) return 0xfff8f0;
			if (stricmp(strColor, "lavenderblush")==0) return 0xf5f0ff;
			if (stricmp(strColor, "seashell")==0) return 0xeef5ff;
			if (stricmp(strColor, "whitesmoke")==0) return 0xf5f5f5;
			if (stricmp(strColor, "honeydew")==0) return 0xf0fff0;
			if (stricmp(strColor, "lightyellow")==0) return 0xe0ffff;
			if (stricmp(strColor, "lightcyan")==0) return 0xffffe0;
			if (stricmp(strColor, "oldlace")==0) return 0xe6f5fd;
			if (stricmp(strColor, "cornsilk")==0) return 0xdcf8ff;
			if (stricmp(strColor, "linen")==0) return 0xe6f0fa;
			if (stricmp(strColor, "lemonchiffon")==0) return 0xcdfaff;
			if (stricmp(strColor, "lightgoldenrodyellow")==0) return 0xd2fafa;
			if (stricmp(strColor, "beige")==0) return 0xdcf5f5;
			if (stricmp(strColor, "lavender")==0) return 0xfae6e6;
			if (stricmp(strColor, "mistyrose")==0) return 0xe1e4ff;
			if (stricmp(strColor, "papayawhip")==0) return 0xd5efff;
			if (stricmp(strColor, "antiquewhite")==0) return 0xd7ebfa;
			if (stricmp(strColor, "blanchedalmond")==0) return 0xcdebff;
			if (stricmp(strColor, "bisque")==0) return 0xc4e4ff;
			if (stricmp(strColor, "moccasin")==0) return 0xb5e4ff;
			if (stricmp(strColor, "gainsboro")==0) return 0xdcdcdc;
			if (stricmp(strColor, "peachpuff")==0) return 0xb9daff;
			if (stricmp(strColor, "paleturquoise")==0) return 0xeeeeaf;
			if (stricmp(strColor, "navajowhite")==0) return 0xaddeff;
			if (stricmp(strColor, "pink")==0) return 0xcbc0ff;
			if (stricmp(strColor, "wheat")==0) return 0xb3def5;
			if (stricmp(strColor, "palegoldenrod")==0) return 0xaae8ee;
			if (stricmp(strColor, "lightgrey")==0) return 0xd3d3d3;
			if (stricmp(strColor, "lightpink")==0) return 0xc1b6ff;
			if (stricmp(strColor, "powderblue")==0) return 0xe6e0b0;
			if (stricmp(strColor, "thistle")==0) return 0xd8bfd8;
			if (stricmp(strColor, "lightblue")==0) return 0xe6d8ad;
			if (stricmp(strColor, "khaki")==0) return 0x8ce6f0;
			if (stricmp(strColor, "violet")==0) return 0xee82ee;
			if (stricmp(strColor, "plum")==0) return 0xdda0dd;
			if (stricmp(strColor, "lightsteelblue")==0) return 0xdec4b0;
			if (stricmp(strColor, "aquamarine")==0) return 0xd4ff7f;
			if (stricmp(strColor, "lightskyblue")==0) return 0xface87;
			if (stricmp(strColor, "silver")==0) return 0xc0c0c0;
			if (stricmp(strColor, "skyblue")==0) return 0xebce87;
			if (stricmp(strColor, "palegreen")==0) return 0x98fb98;
			if (stricmp(strColor, "orchid")==0) return 0xd670da;
			if (stricmp(strColor, "burlywood")==0) return 0x87b8de;
			if (stricmp(strColor, "hotpink")==0) return 0xb469ff;
			if (stricmp(strColor, "lightsalmon")==0) return 0x7aa0ff;
			if (stricmp(strColor, "tan")==0) return 0x8cb4d2;
			if (stricmp(strColor, "lightgreen")==0) return 0x90ee90;
			if (stricmp(strColor, "yellow")==0) return 0x00ffff;
			if (stricmp(strColor, "fuchsia")==0) return 0xff00ff;
			if (stricmp(strColor, "magenta")==0) return 0xff00ff;
			if (stricmp(strColor, "aqua")==0) return 0xffff00;
			if (stricmp(strColor, "cyan")==0) return 0xffff00;
			if (stricmp(strColor, "darkgray")==0) return 0xa9a9a9;
			if (stricmp(strColor, "darksalmon")==0) return 0x7a96e9;
			if (stricmp(strColor, "sandybrown")==0) return 0x60a4f4;
			if (stricmp(strColor, "lightcoral")==0) return 0x8080f0;
			if (stricmp(strColor, "turquoise")==0) return 0xd0e040;
			if (stricmp(strColor, "salmon")==0) return 0x7280fa;
			if (stricmp(strColor, "cornflowerblue")==0) return 0xed9564;
			if (stricmp(strColor, "mediumturquoise")==0) return 0xccd148;
			if (stricmp(strColor, "mediumorchid")==0) return 0xd355ba;
			if (stricmp(strColor, "darkkhaki")==0) return 0x6bb7bd;
			if (stricmp(strColor, "palevioletred")==0) return 0x9370db;
			if (stricmp(strColor, "mediumpurple")==0) return 0xdb7093;
			if (stricmp(strColor, "mediumaquamarine")==0) return 0xaacd66;
			if (stricmp(strColor, "greenyellow")==0) return 0x2fffad;
			if (stricmp(strColor, "rosybrown")==0) return 0x8f8fbc;
			if (stricmp(strColor, "darkseagreen")==0) return 0x8fbc8f;
			if (stricmp(strColor, "gold")==0) return 0x00d7ff;
			if (stricmp(strColor, "mediumslateblue")==0) return 0xee687b;
			if (stricmp(strColor, "coral")==0) return 0x507fff;
			if (stricmp(strColor, "deepskyblue")==0) return 0xffbf00;
			if (stricmp(strColor, "dodgerblue")==0) return 0xff901e;
			if (stricmp(strColor, "tomato")==0) return 0x4763ff;
			if (stricmp(strColor, "deeppink")==0) return 0x9314ff;
			if (stricmp(strColor, "orange")==0) return 0x00a5ff;
			if (stricmp(strColor, "goldenrod")==0) return 0x20a5da;
			if (stricmp(strColor, "darkturquoise")==0) return 0xd1ce00;
			if (stricmp(strColor, "cadetblue")==0) return 0xa09e5f;
			if (stricmp(strColor, "yellowgreen")==0) return 0x32cd9a;
			if (stricmp(strColor, "lightslategray")==0) return 0x998877;
			if (stricmp(strColor, "darkorchid")==0) return 0xcc3299;
			if (stricmp(strColor, "blueviolet")==0) return 0xe22b8a;
			if (stricmp(strColor, "mediumspringgreen")==0) return 0x9afa00;
			if (stricmp(strColor, "peru")==0) return 0x3f85cd;
			if (stricmp(strColor, "slateblue")==0) return 0xcd5a6a;
			if (stricmp(strColor, "darkorange")==0) return 0x008cff;
			if (stricmp(strColor, "royalblue")==0) return 0xe16941;
			if (stricmp(strColor, "indianred")==0) return 0x5c5ccd;
			if (stricmp(strColor, "gray")==0) return 0x808080;
			if (stricmp(strColor, "slategray")==0) return 0x908070;
			if (stricmp(strColor, "chartreuse")==0) return 0x00ff7f;
			if (stricmp(strColor, "springgreen")==0) return 0x7fff00;
			if (stricmp(strColor, "steelblue")==0) return 0xb48246;
			if (stricmp(strColor, "lightseagreen")==0) return 0xaab220;
			if (stricmp(strColor, "lawngreen")==0) return 0x00fc7c;
			if (stricmp(strColor, "darkviolet")==0) return 0xd30094;
			if (stricmp(strColor, "mediumvioletred")==0) return 0x8515c7;
			if (stricmp(strColor, "mediumseagreen")==0) return 0x71b33c;
			if (stricmp(strColor, "chocolate")==0) return 0x1e69d2;
			if (stricmp(strColor, "darkgoldenrod")==0) return 0x0b86b8;
			if (stricmp(strColor, "orangered")==0) return 0x0045ff;
			if (stricmp(strColor, "dimgray")==0) return 0x696969;
			if (stricmp(strColor, "limegreen")==0) return 0x32cd32;
			if (stricmp(strColor, "crimson")==0) return 0x3c14dc;
			if (stricmp(strColor, "sienna")==0) return 0x2d52a0;
			if (stricmp(strColor, "olivedrab")==0) return 0x238e6b;
			if (stricmp(strColor, "darkmagenta")==0) return 0x8b008b;
			if (stricmp(strColor, "darkcyan")==0) return 0x8b8b00;
			if (stricmp(strColor, "darkslateblue")==0) return 0x8b3d48;
			if (stricmp(strColor, "seagreen")==0) return 0x578b2e;
			if (stricmp(strColor, "olive")==0) return 0x008080;
			if (stricmp(strColor, "purple")==0) return 0x800080;
			if (stricmp(strColor, "teal")==0) return 0x808000;
			if (stricmp(strColor, "red")==0) return 0x0000ff;
			if (stricmp(strColor, "lime")==0) return 0x00ff00;
			if (stricmp(strColor, "blue")==0) return 0xff0000;
			if (stricmp(strColor, "brown")==0) return 0x2a2aa5;
			if (stricmp(strColor, "firebrick")==0) return 0x2222b2;
			if (stricmp(strColor, "darkolivegreen")==0) return 0x2f6b55;
			if (stricmp(strColor, "saddlebrown")==0) return 0x13458b;
			if (stricmp(strColor, "forestgreen")==0) return 0x228b22;
			if (stricmp(strColor, "indigo")==0) return 0x82004b;
			if (stricmp(strColor, "darkslategray")==0) return 0x4f4f2f;
			if (stricmp(strColor, "mediumblue")==0) return 0xcd0000;
			if (stricmp(strColor, "midnightblue")==0) return 0x701919;
			if (stricmp(strColor, "darkred")==0) return 0x00008b;
			if (stricmp(strColor, "darkblue")==0) return 0x8b0000;
			if (stricmp(strColor, "maroon")==0) return 0x000080;
			if (stricmp(strColor, "green")==0) return 0x008000;
			if (stricmp(strColor, "navy")==0) return 0x800000;
			if (stricmp(strColor, "darkgreen")==0) return 0x006400;
			if (stricmp(strColor, "black")==0) return 0x000000;
		}
		return CLR_INVALID;
	}
	TagType GetTagType(LPCSTR tagName) {
		if (stricmp(tagName,"b"			)==0) return TagB;
		if (stricmp(tagName,"/b"		)==0) return TagB_;
		if (stricmp(tagName,"big"		)==0) return TagBIG;
		if (stricmp(tagName,"/big"		)==0) return TagSMALL;
		if (stricmp(tagName,"br"		)==0) return TagBR;
		if (stricmp(tagName,"div"		)==0) return TagDIV;
		if (stricmp(tagName,"/div"		)==0) return TagBR;
		if (stricmp(tagName,"font"		)==0) return TagFONT;
		if (stricmp(tagName,"/font"		)==0) return TagFONT_;
		if (stricmp(tagName,"i"			)==0) return TagI;
		if (stricmp(tagName,"/i"		)==0) return TagI_;
		if (stricmp(tagName,"img"		)==0) return TagIMG;
		if (stricmp(tagName,"s"			)==0) return TagS;
		if (stricmp(tagName,"/s"		)==0) return TagS_;
		if (stricmp(tagName,"small"		)==0) return TagSMALL;
		if (stricmp(tagName,"/small"	)==0) return TagBIG;
		if (stricmp(tagName,"u"			)==0) return TagU;
		if (stricmp(tagName,"/u"		)==0) return TagU_;
		return TagUNKNOWN;
	}
	void EntityReference(char& c){
		if (c=='&') {
			// 実体参照じゃなかったときのために戻り先を記憶しておこうか
			int nBack = m_nSeek;
			char buf[6], d;
			// ５文字見れば十分でち(;も読んでやらんと)
			for (int k=0; k<5; k++) {
				d=Read();
				if ((d<'a'||'z'<d)&&(d<'A'||'Z'<d)) {
					// ;や\0で終わっていれば読み戻さない
					if (d!=';'&&d!='\0') Unread();	
					break;
				}
				buf[k]=d;
			}
			if (k<2 || 4<k) { m_nSeek = nBack; return; }
			buf[k] = '\0';
			if (stricmp(buf,"amp"	)==0) c = '&';
			ef (stricmp(buf,"lt"	)==0) c = '<';
			ef (stricmp(buf,"gt"	)==0) c = '>';
			ef (stricmp(buf,"quot"	)==0) c = '"';
			ef (stricmp(buf,"apos"	)==0) c = '\'';
			ef (stricmp(buf,"nbsp"	)==0) c = ' ';
			else m_nSeek = nBack;
		}
	}

public:
	Parser(Cf3HTMLDIB32*lp) { m_lpHTML = lp; }

	void Parse() {
		// 準備ねー
		stack<Tf3HTMLPiece> fontStack;
		Tf3HTMLPiece fontDefault;
		fontDefault.fontSize = 3;
		fontDefault.rgbColor = 0xffffff;
		fontDefault.bBold = fontDefault.bItalic = fontDefault.bUnderLine =
			fontDefault.bStrikeOut = fontDefault.bImage = fontDefault.bBreak = false;
		Pieces().clear();
		Pieces().push_back(fontDefault);
		Length() = 0;
		vector<char> pureText(String().length()+1),
			innerText(String().length()+1),
			buf(String().length());
		TagType tagType;

		// 解析開始じゃー！
		m_nSeek = 0;
		int j=0, k, l=0;
		Tf3HTMLPiece piece;
		while (char c=Read()) {
			if (c=='\r'||c=='\n') continue;	// BR以外で改行が入るとややこしいのよ～
			if (c=='<') {
				// もしかしてタグなのか！？
				k = 0;
				while (true) {
					c=Read();
					if (c=='>'||c==' '||c=='\r'||c=='\n'||c=='\t'||c=='\0') break;
					buf[k++]=c;
				}
				buf[k]='\0';
				// タグ名がわかったところで扱いやすいよう数値にしておくか
				tagType = GetTagType(buf.begin());
				// 属性を見る前にしておく処理
				if (tagType != TagUNKNOWN) {
					if (l>0) {
						innerText[l] = '\0';
						Current().data = innerText.begin();
						Pieces().push_back(Current());
						l = 0;
					}
					switch (tagType) {
					case TagFONT:
						// 直前のフォントを保管
						fontStack.push(Current());
						break;
					case TagIMG:
						// クリップ範囲とサイズをリセット
						SetRect(&Current().rcClip, -1, -1, -1, -1);
						Current().szImage.cx = Current().szImage.cy = -1;
						break;
					}
				}
				// 属性チェックらぁ～
				while (true) {
					// 空白スキップ！
					while (c==' '||c=='\r'||c=='\n'||c=='\t') c=Read();
					if (c=='>'||c=='\0') break;
					// 属性名を読み込む
					k = 0;	Unread();	// 既に属性名を一文字読み込んでしまったので読み戻す
					while (true) {
						c=Read();
						if (c=='>'||c==' '||c=='\r'||c=='\n'||c=='\t'||c=='\0'||c=='=') break;
						buf[k++]=c;
					}
					buf[k++]='\0';
					char *pAttribute = buf.begin(), *pValue = NULL;
					// 必要なら属性値を読み込む
					if (c=='=') {
						pValue = buf.begin()+k;
						c=Read();
						if (c=='"') {
							// ダブルクォーテーションで囲まれた値なのか
							while (true) {
								c=Read();
								if (c=='\0'||c=='"') break;
								EntityReference(c);
								buf[k++]=c;
							}
						}ef(c=='\'') {
							// シングルクォーテーションもありうるよな
							while (true) {
								c=Read();
								if (c=='\0'||c=='\'') break;
								EntityReference(c);
								buf[k++]=c;
							}
						}else {
							// 囲みなし
							Unread();	// その前に一文字読み戻す
							while (true) {
								c=Read();
								if (c=='>'||c==' '||c=='\r'||c=='\n'||c=='\t'||c=='\0') break;
								EntityReference(c);
								buf[k++]=c;
							}
						}
						buf[k]='\0';
					}
					// さて、属性名と属性値も読み込めたところで実際使ってみるか
					switch (tagType) {
					case TagDIV:
						if (pValue) {
							if (stricmp(pAttribute,"align" )==0) {
								if (stricmp(pValue,"left"  )==0) m_lpHTML->m_Align=AlignLeft;
								ef (stricmp(pValue,"center")==0) m_lpHTML->m_Align=AlignCenter;
								ef (stricmp(pValue,"right" )==0) m_lpHTML->m_Align=AlignRight;
							}ef(stricmp(pAttribute,"valign")==0) {
								if (stricmp(pValue,"bottom")==0) m_lpHTML->m_VAlign=AlignBottom;
								ef (stricmp(pValue,"middle")==0) m_lpHTML->m_VAlign=AlignMiddle;
								ef (stricmp(pValue,"top"   )==0) m_lpHTML->m_VAlign=AlignTop;
							}
						}
						break;
					case TagFONT:
						if (pValue) {
							if (stricmp(pAttribute,"size")==0) {
								switch (*pValue) {
								case '+': Current().fontSize += atoi(pValue+1); break;
								case '-': Current().fontSize -= atoi(pValue+1); break;
								default : Current().fontSize  = atoi(pValue  ); break;
								}
							}ef(stricmp(pAttribute,"color")==0) {
								COLORREF col;
								if ((col=GetColor(pValue))!=CLR_INVALID)
									Current().rgbColor = col;
							}
						}
						break;
					case TagIMG:
						if (pValue) {
							if (stricmp(pAttribute,"src")==0) {
								Current().data = pValue;
							}ef(stricmp(pAttribute,"clip")==0) {
								sscanf(pValue, "%d,%d,%d,%d"
									,&Current().rcClip.left
									,&Current().rcClip.top
									,&Current().rcClip.right
									,&Current().rcClip.bottom
									);
							}ef(stricmp(pAttribute,"width")==0) {
								Current().szImage.cx = atoi(pValue);
							}ef(stricmp(pAttribute,"height")==0) {
								Current().szImage.cy = atoi(pValue);
							}
						}
						break;
					}
				}
				// 最終的な処理
				switch (tagType) {
				case TagB: Current().bBold=true; break;
				case TagB_: Current().bBold=false; break;
				case TagBIG: Current().fontSize++; break;
				case TagBR:
					Current().bBreak = true;
					Pieces().push_back(Current());
					Current().bBreak = false;
					Length()++;
					break;
				case TagFONT_:
					if (fontStack.empty()) {
						Current() = fontDefault;
					}else {
						Current() = fontStack.top();
						fontStack.pop();
					}
					break;
				case TagI: Current().bItalic=true; break;
				case TagI_: Current().bItalic=false; break;
				case TagIMG:
					Current().bImage = true;
					Pieces().push_back(Current());
					Current().bImage = false;
					Length()++;
					break;
				case TagS: Current().bStrikeOut=true; break;
				case TagS_: Current().bStrikeOut=false; break;
				case TagSMALL: Current().fontSize--; break;
				case TagU: Current().bUnderLine=true; break;
				case TagU_: Current().bUnderLine=false; break;
				}
				continue;
			}
			EntityReference(c);
			pureText[j++] = innerText[l++] = c;
			if (_ismbblead(c)) pureText[j++] = innerText[l++] = Read();
			Length()++;
		}
		pureText[j] = '\0';
		m_lpHTML->m_PureText = pureText.begin();
		if (l>0) {
			innerText[l] = '\0';
			Current().data = innerText.begin();
		}else {
			Pieces().pop_back();
		}
		m_lpHTML->m_Rects.resize(Length());
	}
};

class Renderer {
private:
	Cf3HTMLDIB32*m_lpHTML;
	struct LineInfo {
		int left, top, width, height;
	};
	list<LineInfo> m_Lines;
	list<LineInfo>::iterator m_Line;

	list<Tf3HTMLPiece>& Pieces() { return m_lpHTML->m_Pieces; }
	vector<RECT>& Rects() { return m_lpHTML->m_Rects; }
	RECT& Rect(int index) { return m_lpHTML->m_Rects[index]; }
	int Length() { return m_lpHTML->m_nLength; }
	int ByteLength() { return m_lpHTML->m_PureText.size(); }
	const string& String() { return m_lpHTML->m_String; }
	HDC GetDC() { return m_lpHTML->GetDC(); }

	int GetFontSize(int relative) { return m_lpHTML->GetFontSize(relative); }
	int GetMaxWidth() { return m_lpHTML->m_nMaxWidth; }

	void Insert(int w, int h) {
		if ((*m_Line).width+w>GetMaxWidth()) InsertBreak();
		(*m_Line).width += w;
		if ((*m_Line).height<h) (*m_Line).height=h;
	}
	void InsertBreak() {
		switch (m_lpHTML->m_Align) {
		case AlignLeft:   (*m_Line).left = 0; break;
		case AlignCenter: (*m_Line).left = (GetMaxWidth()-(*m_Line).width)/2; break;
		case AlignRight:  (*m_Line).left = GetMaxWidth()-(*m_Line).width; break;
		}
		(*m_Line).width = 0;
		LineInfo l = { 0, (*m_Line).top+(*m_Line).height, 0, 0};
		m_Lines.push_back(l);
		m_Line++;
	}
	void Place(int w, int h, int&x, int&y) {
		if ((*m_Line).width+w>GetMaxWidth()) {
			m_Line++;
		}
		x = (*m_Line).left+(*m_Line).width;
		(*m_Line).width+=w;
		switch (m_lpHTML->m_VAlign) {
		case AlignBottom: y = (*m_Line).top+(*m_Line).height-h; break;
		case AlignMiddle: y = (*m_Line).top+((*m_Line).height-h)/2; break;
		case AlignTop:    y = (*m_Line).top; break;
		}
	}

public:
	Renderer(Cf3HTMLDIB32*lp) { m_lpHTML = lp; }

	void Render() {
		HFONT hFont;
		bool bFirst;
		m_lpHTML->CreateSurface(1, 1);
		// サイズ計算だべ。
		int w, h;
		LineInfo l = {0};
		vector<char> buf(Length()+1);
		vector<int> aDx(ByteLength()+1);
		m_Lines.clear();
		m_Lines.push_back(l);
		m_Line = m_Lines.begin();
		for (list<Tf3HTMLPiece>::iterator it=Pieces().begin(); it!=Pieces().end(); it++) {
			if ((*it).bImage){
				CDIB32 *pDIB=ResourceManager.Get(atoi((*it).data.c_str()));
				//CDIBResource dib((*it).data);
				if (pDIB) {
					pDIB->GetSize(w, h);
					RECT& rc = (*it).rcClip;
					if (rc.left  <0 || w<rc.left  ) rc.left   = 0;
					if (rc.top   <0 || h<rc.top   ) rc.top    = 0;
					if (rc.right <0 || w<rc.right ) rc.right  = w;
					if (rc.bottom<0 || h<rc.bottom) rc.bottom = h;
					(*it).szImage.cx = w =
						(*it).szImage.cx>=0?(*it).szImage.cx:(rc.right - rc.left);
					(*it).szImage.cy = h =
						(*it).szImage.cy>=0?(*it).szImage.cy:(rc.bottom - rc.top);
					Insert(w, h);
				}
			}else if((*it).bBreak) {
				if (m_Lines.back().height==0)
					m_Lines.back().height = GetFontSize((*it).fontSize);
				InsertBreak();
			}else {
				hFont = CreateFont(GetFontSize((*it).fontSize),0,0,0,
					(*it).bBold?FW_BOLD:FW_LIGHT,
					(*it).bItalic,
					(*it).bUnderLine,
					(*it).bStrikeOut,
					SHIFTJIS_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,FF_MODERN,"ＭＳ ゴシック");
				HFONT hFontLast = (HFONT)::SelectObject(GetDC(),hFont);
				int offset=0, len=(*it).data.length(), fit=0;
				SIZE size;
				bFirst = true;
				while (offset<len) {
					if (!bFirst) InsertBreak();
					GetTextExtentExPoint(GetDC(),
						(*it).data.c_str()+offset,
						len-offset,
						GetMaxWidth()-(*m_Line).width,
						&fit,
						aDx.begin(),
						&size);
					// 最低一文字は進めないと無限ループになる
					if (fit==0) {
						if (bFirst) { bFirst=false; continue; }
						else fit=_ismbblead((*it).data.c_str()[offset])?2:1;
					}
					offset += fit;
					Insert(aDx[fit-1], size.cy);
					bFirst=false;
				}
				::SelectObject(GetDC(),hFontLast);
				DeleteObject(hFont);
			}
		}
		InsertBreak();

		// ついに画像生成だべ。
		m_lpHTML->CreateSurface(GetMaxWidth(), max(1,m_Lines.back().top+m_Lines.back().height));

		// 描くぞー書くぞー！
		int pos = 0;
		m_Line = m_Lines.begin();
		int x=(*m_Line).left, y;
		for (it=Pieces().begin(); it!=Pieces().end(); it++) {
			if ((*it).bImage){
				//CDIBResource dib((*it).data);
				CDIB32 *pDIB=ResourceManager.Get(atoi((*it).data.c_str()));
				if (pDIB) {
					RECT& rc = (*it).rcClip;
					w = rc.right - rc.left;
					h = rc.bottom - rc.top;
					Place((*it).szImage.cx, (*it).szImage.cy, x, y);
					m_lpHTML->BltNatural(pDIB, x, y, &rc, &(*it).szImage);
					SetRect(&Rects()[pos++], 0, (*m_Line).top,
						x+w, (*m_Line).top+(*m_Line).height);
				}
			}else if((*it).bBreak) {
				m_Line++;
				SetRect(&Rects()[pos++], 0, (*m_Line).top, 0, (*m_Line).top+(*m_Line).height);
			}else {
				hFont = CreateFont(GetFontSize((*it).fontSize),0,0,0,
					(*it).bBold?FW_BOLD:FW_LIGHT,
					(*it).bItalic,
					(*it).bUnderLine,
					(*it).bStrikeOut,
					SHIFTJIS_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,FF_MODERN,"ＭＳ ゴシック");
				HFONT hFontLast = (HFONT)::SelectObject(GetDC(),hFont);
				int offset=0, len=(*it).data.length(), fit;
				SIZE size;
				bFirst = true;
				while (offset<len) {
					if (!bFirst) m_Line++;
					GetTextExtentExPoint(GetDC(),
						(*it).data.c_str()+offset,
						len-offset,
						GetMaxWidth()-(*m_Line).width,
						&fit,
						aDx.begin(),
						&size);
					// 最低一文字は進めないと無限ループになる
					if (fit==0) {
						if (bFirst) { bFirst=false; continue; }
						else fit=_ismbblead((*it).data.c_str()[offset])?2:1;
					}
					Place(aDx[fit-1], size.cy, x, y);
					for (int i=0; i<fit; i++) {
						if (_ismbblead((*it).data.c_str()[offset+i])) i++;
						SetRect(&Rects()[pos++], 0, (*m_Line).top, min(x+aDx[i], GetMaxWidth()), (*m_Line).top+(*m_Line).height);
					}
					SetBkMode(GetDC(), TRANSPARENT);
					SetTextColor(GetDC(),(*it).rgbColor);
					TextOut(GetDC(), x, y, (*it).data.c_str()+offset, fit);
					offset += fit;
					bFirst=false;
				}
				::SelectObject(GetDC(),hFontLast);
				DeleteObject(hFont);
			}
		}
	}
};


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3HTMLDIB32::Cf3HTMLDIB32()
{
	m_bDirty = true;
	m_String = "";
	m_nFontSize = 16;
	m_nMaxWidth = 320;
	m_nLength = 0;
	m_DefaultAlign = AlignLeft;
	m_DefaultVAlign = AlignBottom;
	UseDIBSection(true);
}

Cf3HTMLDIB32::~Cf3HTMLDIB32()
{
}

void Cf3HTMLDIB32::SetText(const string &s)
{
	if (m_String != s) {
		m_String = s;
		UpdateCondition();
		m_bDirty = true;
	}
}

void __cdecl Cf3HTMLDIB32::SetText(LPSTR fmt, ... )
{
	CHAR buf[512];
	wvsprintf(buf,fmt,(LPSTR)(&fmt+1));
	SetText((string)buf);
}

void Cf3HTMLDIB32::Update()
{
	if (!m_bDirty) return;
	
	Renderer(this).Render();
	m_bDirty = false;
}

void Cf3HTMLDIB32::UpdateCondition()
{
	m_Align = m_DefaultAlign;
	m_VAlign = m_DefaultVAlign;
	Parser(this).Parse();
}

void Cf3HTMLDIB32::SetFontSize(int nSize)
{
	if (m_nFontSize != nSize) {
		m_nFontSize = nSize;
		m_bDirty = true;
	}
}

int Cf3HTMLDIB32::GetFontSize(int relative)
{
	switch (relative) {
	case 1: return m_nFontSize*3/5;
	case 2: return m_nFontSize*4/5;
	case 3: return m_nFontSize;
	case 4: return m_nFontSize*6/5;
	case 5: return m_nFontSize*3/2;
	case 6: return m_nFontSize*2;
	case 7: return m_nFontSize*3;
	default: return relative<=0
				 ?max(1,GetFontSize(relative+1)*10/11)
				 :min(m_nMaxWidth,GetFontSize(relative-1)*11/10);
	}
}

void Cf3HTMLDIB32::SetMaxWidth(int w)
{
	if (m_nMaxWidth != w) {
		m_nMaxWidth = w;
		m_bDirty = true;
	}
}

void Cf3HTMLDIB32::OnDraw(CDIB32 *lp, int x, int y, int length)
{
	Update();
	if (length<=0) return;
	if (length>=m_nLength) { InnerBlt(this, x, y); return; }
	RECT& rcSrc = m_Rects[length-1];
	if (rcSrc.top>0) {
		RECT rcSrc2 = { 0, 0, m_nMaxWidth, rcSrc.top, };
		InnerBlt(this, x, y, &rcSrc2);
	}
	InnerBlt(this, x, y, &rcSrc);
}

void Cf3HTMLDIB32::InnerBlt(CDIB32 *lp, int x, int y, LPRECT rc)
{
	lp->BltNatural(this, x, y, rc);
}
