// f3MapObjectmrframe.h: Cf3MapObjectmrframe クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAPOBJECTMRFRAME_H__41085B33_9387_4A3A_9607_5433EC99D85E__INCLUDED_)
#define AFX_F3MAPOBJECTMRFRAME_H__41085B33_9387_4A3A_9607_5433EC99D85E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f3MapObjectBase.h"
#include "f3MapObjectfunya.h"

struct f3JumpFunction;

class Cf3MapObjectmrframe : public Cf3MapObjectBase 
{
protected:
/*	static CDIB32		m_Graphic;
	static bool			m_bGraphicInitialize;
	CDIBResource m_Graphic;*/
	CDIB32* m_Graphic;

	Cf3MapObjectfunya	*m_funya;
	int					m_nLife;
public:
	void UpdateCPos();
	bool IsFrozen();
	static set<Cf3MapObjectmrframe*>	m_EnemyList;
	static void OnDrawAll(CDIB32 *lp);
	static void OnPreDrawAll();
	static void SynergyAll();
	static void OnMoveAll();
	static set<Cf3MapObjectmrframe*>::iterator IteratorBegin() { return m_EnemyList.begin(); }
	static set<Cf3MapObjectmrframe*>::iterator IteratorEnd(){ return m_EnemyList.end(); }
	void Synergy();
	void OnPreDraw();
	void OnMove();
	void OnDraw(CDIB32 *lp);
	Cf3MapObjectmrframe(int nCX, int nCY);
	virtual ~Cf3MapObjectmrframe();

};

#endif // !defined(AFX_F3MAPOBJECTMRFRAME_H__41085B33_9387_4A3A_9607_5433EC99D85E__INCLUDED_)
