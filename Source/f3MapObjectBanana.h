// f3MapObjectBanana.h: Cf3MapObjectBanana クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAPOBJECTBANANA_H__523B95D8_4CE2_4CD3_A069_7BDDA3E30C33__INCLUDED_)
#define AFX_F3MAPOBJECTBANANA_H__523B95D8_4CE2_4CD3_A069_7BDDA3E30C33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f3MapObjectBase.h"

class Cf3MapObjectBanana : public Cf3MapObjectBase  
{
protected:
	CDIB32* m_Graphic;
	static set<Cf3MapObjectBanana*> m_BananaList;
public:
	void UpdateCPos() {}
	static void OnPreDrawAll();
	static void SynergyAll();
	static void OnDrawAll(CDIB32 *lp);
	static set<Cf3MapObjectBanana*>::iterator IteratorBegin() { return m_BananaList.begin(); }
	static set<Cf3MapObjectBanana*>::iterator IteratorEnd(){ return m_BananaList.end(); }
	void Reaction(Cf3MapObjectBase *obj);
	void Synergy();
	void OnDraw(CDIB32* lp);
	Cf3MapObjectBanana(int nCX, int nCY);
	virtual ~Cf3MapObjectBanana();

};

#endif // !defined(AFX_F3MAPOBJECTBANANA_H__523B95D8_4CE2_4CD3_A069_7BDDA3E30C33__INCLUDED_)
