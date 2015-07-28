// f3MapObjectItem.h: Cf3MapObjectItem クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAPOBJECTITEM_H__C99AA24A_060A_4B86_A894_1909DA60EE83__INCLUDED_)
#define AFX_F3MAPOBJECTITEM_H__C99AA24A_060A_4B86_A894_1909DA60EE83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f3MapObjectBase.h"

class Cf3MapObjectItem : public Cf3MapObjectBase  
{
protected:
public:
	static void Garbage();
	static void KillAll();
	static void OnPreDrawAll();
	static void OnDrawAll(CDIB32 *lp);
	static void OnMoveAll();
	Cf3MapObjectItem(f3MapObjectType eType);
	virtual ~Cf3MapObjectItem();

	static set<Cf3MapObjectItem*>	m_ItemList;
};

#endif // !defined(AFX_F3MAPOBJECTITEM_H__C99AA24A_060A_4B86_A894_1909DA60EE83__INCLUDED_)
