// f3MapObjectIceSource.h: Cf3MapObjectIceSource クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAPOBJECTICESOURCE_H__FA7F8646_2D66_4FC5_AC34_94B38081C4D2__INCLUDED_)
#define AFX_F3MAPOBJECTICESOURCE_H__FA7F8646_2D66_4FC5_AC34_94B38081C4D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f3MapObjectIceBase.h"

class Cf3MapObjectIceSource : public Cf3MapObjectIceBase  
{
protected:
	static set<Cf3MapObjectIceSource*> m_IceList;
	int m_Phase;
	int m_Size;
public:
	static set<Cf3MapObjectIceSource*>::iterator IteratorBegin() { return m_IceList.begin(); }
	static set<Cf3MapObjectIceSource*>::iterator IteratorEnd(){ return m_IceList.end(); }
	int GetSize();
	static void OnPreDrawAll();
	static void OnDrawAll(CDIB32 *lp);
	void OnPreDraw();
	void OnDraw(CDIB32 *lp);
	Cf3MapObjectIceSource(int x, int y);
	virtual ~Cf3MapObjectIceSource();

};

#endif // !defined(AFX_F3MAPOBJECTICESOURCE_H__FA7F8646_2D66_4FC5_AC34_94B38081C4D2__INCLUDED_)
