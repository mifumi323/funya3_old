// f3MapObjectIce.h: Cf3MapObjectIce クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAPOBJECTICE_H__F4FB5DE8_B6AB_4715_A754_2A14CCE8E504__INCLUDED_)
#define AFX_F3MAPOBJECTICE_H__F4FB5DE8_B6AB_4715_A754_2A14CCE8E504__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f3MapObjectIceBase.h"

class Cf3MapObjectIce : public Cf3MapObjectIceBase  
{
protected:
	static set<Cf3MapObjectIce*>	Cf3MapObjectIce::m_IceList;

	float m_DX, m_DY;
	int m_Life;
public:
	int GetSize();
	static set<Cf3MapObjectIce*>::iterator IteratorBegin() { return m_IceList.begin(); }
	static set<Cf3MapObjectIce*>::iterator IteratorEnd(){ return m_IceList.end(); }
	static void OnPreDrawAll();
	static void SynergyAll();
	static void OnMoveAll();
	static void OnDrawAll(CDIB32 *lp);
	void OnDraw(CDIB32 *lp);
	void OnMove();
	Cf3MapObjectIce(float x, float y, float dx, float dy);
	virtual ~Cf3MapObjectIce();

};

#endif // !defined(AFX_F3MAPOBJECTICE_H__F4FB5DE8_B6AB_4715_A754_2A14CCE8E504__INCLUDED_)
