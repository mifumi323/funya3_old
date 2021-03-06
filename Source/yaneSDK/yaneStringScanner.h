//	文字列解析アシスト関数

#ifndef __yaneStringScanner_h__
#define __yaneStringScanner_h__

class CStringScanner {
/**
	文字列の解析のサポートをします。
	class CTextDraw,class CScenarioView	で
	実際に使っているので、その実装を見ると参考になるでしょう。
*/
public:
static	bool	IsToken(LPCSTR &lp,LPCSTR lp2);
	///	トークンチェック
	///	一致後は、その分だけlpは前進する

static	LRESULT	SkipTo(LPCSTR &pSrcStr, LPCSTR pTokenStr);
	///	指定トークンまで読み飛ばし
	///	一致後は、その分だけlpは前進する。'\0'に出くわしたら非0が返る

static	LRESULT	SkipTo2(LPCSTR &pSrcStr, LPCSTR pTokenStr, LPSTR pSkipedStr);
	///	指定トークンまで読み飛ばし
	///	一致後は、その分だけlpは前進する。'\0'に出くわしたら非0が返る
	///	lp3(bufは呼び出し側で用意すること)には、スキップするまでの文字列が入る

static	LRESULT SkipSpace(LPCSTR &lp);
	///	スペース、タブ、改行を切り詰める
	///	'\0'に出くわしたら非0が返る

static	LRESULT SkipSpace2(LPCSTR &lp);
	///	スペース、タブ、改行、カンマを切り詰める
	///	'\0'に出くわしたら非0が返る

static	LRESULT	GetStrNum(LPCSTR &lp,int& nRate);
	///	"+1"という文字列ならばnRateには1が返る

static	LRESULT	GetNum(LPCSTR &lp,int& nVal);
	///	+1という文字列ならばnValには1が返る

static	LRESULT GetStrColor(LPCSTR &lp,COLORREF& m_nFontColor);
	/// "#ff0000"という文字列ならばRGB(255,0,0)が返る

static	string GetStr(LPCSTR &lp,CHAR c);
	///	cに遭遇するところまでの文字列を返す

static	string GetStr(LPCSTR &lp);
	///	スペース、タブ、改行に遭遇するまでの文字列を返す

static	string GetStrFileName(LPCSTR &lp);
	///	タブ、改行に遭遇するまでの文字列を返す

static	string GetNextStr(LPCSTR &lp);
///	最初に遭遇したスペース、タブ、改行を無視し、そのあとスペース、タブ、
///	改行に遭遇するまでの文字列を返す
///	ただし、"　で始まっている場合は、再度"に遭遇するまでをその文字列として返す。
///	（このとき" は自動的に取り除かれる）

static LRESULT GetNumFromCsv(LPCSTR &lp,int& nVal,bool bSkip=false);
///	カンマ、スペースを無視しながら数値を読み込むのだ
static string GetStrFromCsv(LPCSTR &lp);
///	カンマ、スペースを無視しながら文字列を読み込むのだ。
/**
	カンマ、スペースを無視しながら数値を読み込む。
	CSVというのは、Microsoft Excelなどで出力できるカンマ区切りのテキスト形式。
	bSkip == trueの場合、GetNumFromCsv/GetStrFromCsvをしたあとは必ず次の,は
	無くなります。(bSkip==falseならば、無くなりません)
	1,2,3で一回Getすると2,3になります。
	,,2,3ならば、エラーが返し値として返り、次の,が無くなり、,2,3となります。
*/

static void	Replace(string& buf,LPCSTR pSrc,LPCSTR pDst,bool bCaseSensitive=true);
///	string中に含まれるpSrcと一致する部分をpDstに置換する
///	日本語（マルチバイト文字）対応。bCaseSensitive==trueのときは、
///	アルファベットの大文字小文字を区別する。

///		数字と文字列の相互変換関数
///		先頭のスペースは無視しない
static LRESULT	StringToNum(string s,LONGLONG& l);
static void NumToString(LONGLONG l,string& s);
static string NumToString(LONGLONG l) { string s; NumToString(l,s); return s;}
//	↓こちらは、ゼロサプレスして、n桁にする。
static void NumToStringZ(LONGLONG l,string& s,int n);
static string NumToStringZ(LONGLONG l,int n) { string s; NumToStringZ(l,s,n); return s;}
};

#endif