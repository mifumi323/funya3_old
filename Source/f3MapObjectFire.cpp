// f3MapObjectFire.cpp: Cf3MapObjectFire クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3Map.h"
#include "f3MapObjectMain.h"
#include "f3MapObjectmrframe.h"
#include "f3MapObjectFire.h"
#include "f3MapObjectIce.h"
#include "App.h"

set<Cf3MapObjectFire*> Cf3MapObjectFire::m_FireList;

#define PHASEMAX	32

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3MapObjectFire::Cf3MapObjectFire(int x, int y)
{
	m_FireList.insert(this);
	SetPos(x*32+16,y*32+16);
	SetID(OID_FIRE);
	m_Delay = 0;
	m_Phase = CApp::random(PHASEMAX);
	m_Size = GetSize();
}

Cf3MapObjectFire::~Cf3MapObjectFire()
{
	m_FireList.erase(this);
}

void Cf3MapObjectFire::OnDrawAll(CDIB32 *lp)
{
	for(set<Cf3MapObjectFire*>::iterator it = m_FireList.begin();it!=m_FireList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnDraw(lp);
	}
}

void Cf3MapObjectFire::OnMoveAll()
{
	for(set<Cf3MapObjectFire*>::iterator it = m_FireList.begin();it!=m_FireList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnMove();
	}
}

void Cf3MapObjectFire::OnPreDrawAll()
{
	for(set<Cf3MapObjectFire*>::iterator it = m_FireList.begin();it!=m_FireList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnPreDraw();
	}
}

void Cf3MapObjectFire::SynergyAll()
{
	for(set<Cf3MapObjectFire*>::iterator it = m_FireList.begin();it!=m_FireList.end();it++){
		if ((*it)->m_bValid)
			(*it)->Synergy();
	}
}

int Cf3MapObjectFire::GetSize()
{
	int s=abs((PHASEMAX/2)-m_Phase)*6/PHASEMAX;
	if (m_Delay==0) s+=10;
	return s;
}

void Cf3MapObjectFire::OnDraw(CDIB32 *lp)
{
	if (!IsValid()) return;
	RECT rc = { (15-m_Size)*64, 64, (16-m_Size)*64, 128, };
	SetViewPos(-32,-32);
	lp->BltNatural(m_Graphic,m_nVX,m_nVY,&rc);
}

void Cf3MapObjectFire::OnMove()
{
	if (m_Delay>0) m_Delay--;
	ef (m_Delay<0) m_Delay++;
}

void Cf3MapObjectFire::OnPreDraw()
{
	if (CApp::random(40)) { m_Phase++; m_Phase%=PHASEMAX; }
	if (m_Size<GetSize()) { m_Size++; }
	ef (m_Size>GetSize()) { m_Size--; }
}

bool Cf3MapObjectFire::IsActive()
{
	return m_Delay==0;
}

void Cf3MapObjectFire::Synergy()
{
	if (m_Delay==0) {
		// ふにゃ
		if (m_pParent->GetMainChara()->IsFrozen()) {
			float objX, objY;
			m_pParent->GetMainChara()->GetPos(objX,objY);
			if ((objX-m_X)*(objX-m_X)+(objY-m_Y)*(objY-m_Y)<256) {
				// あたった！
				m_Delay = 200;
			}
		}
		for(set<Cf3MapObjectmrframe*>::iterator mf = Cf3MapObjectmrframe::IteratorBegin();
		mf!=Cf3MapObjectmrframe::IteratorEnd();mf++){
			if ((*mf)->IsValid()&&(*mf)->IsFrozen()){
				float objX, objY;
				(*mf)->GetPos(objX,objY);
				if ((objX-m_X)*(objX-m_X)+(objY-m_Y)*(objY-m_Y)<256) {
					// あたった！
					m_Delay = 200;
				}
			}
		}
		// 氷
		for(set<Cf3MapObjectIce*>::iterator ic = Cf3MapObjectIce::IteratorBegin();
		ic!=Cf3MapObjectIce::IteratorEnd();ic++){
			if ((*ic)->IsValid()) {
				float objX, objY;
				(*ic)->GetPos(objX,objY);
				if ((objX-m_X)*(objX-m_X)+(objY-m_Y)*(objY-m_Y)<256) {
					// あたった！
					m_Delay = 200;
				}
			}
		}
	}
}
