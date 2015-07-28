// f3MapObjectNeedle.h: Cf3MapObjectNeedle クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAPOBJECTNEEDLE_H__FDE5D780_A485_4EE6_B87F_080DBBCEA78E__INCLUDED_)
#define AFX_F3MAPOBJECTNEEDLE_H__FDE5D780_A485_4EE6_B87F_080DBBCEA78E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f3MapObjectBase.h"

class Cf3MapObjectNeedle : public Cf3MapObjectBase  
{
protected:
	void Reaction(Cf3MapObjectBase *obj);
/*	static CDIB32	m_Graphic;
	static bool		m_bGraphicInitialize;
	CDIBResource m_Graphic;*/
	CDIB32* m_Graphic;
	static set<Cf3MapObjectNeedle*> m_EnemyList;

	float			m_StartY;
	float			m_Speed;
	enum {
		NDT_UNDEFINED,
		NDT_HORIZONTAL,
		NDT_VERTICAL,
		NDT_DEAD,
	} m_Type;
	enum {
		NDS_WAIT,
		NDS_STOP,
		NDS_LEFT,
		NDS_RIGHT,
		NDS_DOWN,
		NDS_UP,
	} m_State;
public:
	static void OnDrawAll(CDIB32 *lp);
	static void OnPreDrawAll();
	static void SynergyAll();
	static void OnMoveAll();
	static set<Cf3MapObjectNeedle*>::iterator IteratorBegin() { return m_EnemyList.begin(); }
	static set<Cf3MapObjectNeedle*>::iterator IteratorEnd(){ return m_EnemyList.end(); }
	void Synergy();
	virtual void OnMove();
	virtual void OnDraw(CDIB32 *lp);
	Cf3MapObjectNeedle(int nCX, int nCY, int nType=0);
	virtual ~Cf3MapObjectNeedle();

};

#endif // !defined(AFX_F3MAPOBJECTNEEDLE_H__FDE5D780_A485_4EE6_B87F_080DBBCEA78E__INCLUDED_)
