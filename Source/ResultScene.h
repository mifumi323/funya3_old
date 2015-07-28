// ResultScene.h: CResultScene クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULTSCENE_H__049BB4A1_CF47_4DD4_BEA8_9F5348D69E46__INCLUDED_)
#define AFX_RESULTSCENE_H__049BB4A1_CF47_4DD4_BEA8_9F5348D69E46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneBase.h"

typedef pair<string, string> strstrpair;
typedef list<strstrpair> strstrlist;
typedef strstrlist::iterator strstriterator;

class CResultScene : public CSceneBase  
{
protected:
//	Cf3NavigationBar	*m_NavigationBar;
	TTextNavi			m_TextNavi;

	CTextDIB32			*m_TextTitle;
	CDIB32				*m_Icon;
	CTextDIB32			*m_TextResult;

	strstrlist m_Lines;
	strstriterator m_itBegin;
	int m_nLines, m_nScroll, m_nFocus;
	int m_nIcon;

public:
	string SecToStr(int sec);
	void Update();
	void __cdecl Add(string summary, string description, ...);
	void OnDraw(CDIB32 *lp);
	void OnInit();
	CResultScene();
	virtual ~CResultScene();

};

#endif // !defined(AFX_RESULTSCENE_H__049BB4A1_CF47_4DD4_BEA8_9F5348D69E46__INCLUDED_)
