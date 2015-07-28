// f3NavigationBar.h: Cf3NavigationBar クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3NAVIGATIONBAR_H__EE5686CF_4B7B_48CE_B6CD_5CED4C4A3140__INCLUDED_)
#define AFX_F3NAVIGATIONBAR_H__EE5686CF_4B7B_48CE_B6CD_5CED4C4A3140__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "yaneSDK/yaneTextDIB32.h"

class TNavi {
public:
	TNavi() { dib=NULL; offset=16; rect.left=rect.top=rect.right=rect.bottom=0; }
	CDIB32*	dib;
	float	offset;
	RECT	rect;
};

class TTextNavi : public TNavi {
public:
	TTextNavi() { text=NULL; }
	void Update() { text->UpdateText(); int w,h; text->GetSize(w,h); rect.right=w; rect.bottom=h; }
	void Set(CTextDIB32* text) { dib=this->text=text; }
	CTextDIB32* text;
};

class Cf3NavigationBar  
{
protected:
	CDIB32				*m_Back;
	list<TNavi*>		m_PlaneList;

	float	m_Scroll;
	float	m_ScrollTo;
public:
	void Delete(TNavi *lpPlane);
	void Add(TNavi *lpPlane);
	void OnDraw(CDIB32 *lp);
	Cf3NavigationBar();
	virtual ~Cf3NavigationBar();

};

#endif // !defined(AFX_F3NAVIGATIONBAR_H__EE5686CF_4B7B_48CE_B6CD_5CED4C4A3140__INCLUDED_)
