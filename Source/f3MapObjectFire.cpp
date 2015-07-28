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
	:Cf3MapObjectIceBase(MOT_FIRE)
	,m_Delay(0)
{
	m_FireList.insert(this);
	SetPos(x*32+16,y*32+16);
	m_Phase = CApp::random(PHASEMAX);
	m_Size = GetSize();
}

Cf3MapObjectFire::~Cf3MapObjectFire()
{
	m_FireList.erase(this);
}

void Cf3MapObjectFire::OnDrawAll(CDIB32 *lp)
{
	int sx, sy, ex, ey;
	sx = sy = 0;
	m_pParent->GetViewPos(sx,sy);
	sx = (-sx)>>5; sy = (-sy)>>5;
	ex = sx+320/32; ey = sy+224/32;
	Saturate(sx,ex,m_pParent->GetWidth()-1);
	Saturate(sy,ey,m_pParent->GetHeight()-1);
	for (Cf3MapObjectBase**it=m_pParent->GetMapObjects(sx-1, sy-1, ex+1, ey+1, MOT_FIRE); (*it)!=NULL; it++) {
		if ((*it)->IsValid()) (*it)->OnDraw(lp);
	}
}

void Cf3MapObjectFire::OnMoveAll()
{
	for(set<Cf3MapObjectFire*>::iterator it = m_FireList.begin();it!=m_FireList.end();it++){
		if ((*it)->IsValid()) (*it)->OnMove();
	}
}

void Cf3MapObjectFire::OnPreDrawAll()
{
	for(set<Cf3MapObjectFire*>::iterator it = m_FireList.begin();it!=m_FireList.end();it++){
		if ((*it)->IsValid()) (*it)->OnPreDraw();
	}
}

void Cf3MapObjectFire::SynergyAll()
{
	for(set<Cf3MapObjectFire*>::iterator it = m_FireList.begin();it!=m_FireList.end();it++){
		if ((*it)->IsValid()) (*it)->Synergy();
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

void Cf3MapObjectFire::Synergy()
{
	if (m_Delay==0) {
		// ふにゃ
		Cf3MapObjectBase**it;
		for(it=m_pParent->GetMapObjects(m_nCX-1, m_nCY-1, m_nCX+1, m_nCY+1, MOT_FUNYA); (*it)!=NULL; it++){
			if ((*it)->IsValid()&&((Cf3MapObjectMain*)(*it))->IsFrozen()) {
				float objX, objY;
				(*it)->GetPos(objX,objY);
				// あたった！
				if ((objX-m_X)*(objX-m_X)+(objY-m_Y)*(objY-m_Y)<256) m_Delay = 200;
			}
		}
		// 氷
		for(it=m_pParent->GetMapObjects(m_nCX-1, m_nCY-1, m_nCX+1, m_nCY+1, MOT_ICE); (*it)!=NULL; it++){
			if ((*it)->IsValid()) {
				float objX, objY;
				(*it)->GetPos(objX,objY);
				// あたった！
				if ((objX-m_X)*(objX-m_X)+(objY-m_Y)*(objY-m_Y)<256) m_Delay = 200;
			}
		}
	}
}
