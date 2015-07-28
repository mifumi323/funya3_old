// f3MapObjectIce.cpp: Cf3MapObjectIce クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectIce.h"
#include "f3Map.h"
#include "yaneSDK/yaneDIB32.h"

set<Cf3MapObjectIce*> Cf3MapObjectIce::m_IceList;

const float RUNMAX = 13.0f;
const float RUNFRICTION = 0.43f;
const float WALKACCEL = 0.4f;
const float WALKFRICTION = 0.24f;
const float JUMPACCEL = 0.12f;
const float JUMPMAX = 13.0f;
const float FALLMAX = 13.0f;
const float GRAVITY = 0.2f;
const float FRICTION = 0.026f;
const float REFRECTION = 0.9f;
const int LIFE = 160;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3MapObjectIce::Cf3MapObjectIce(float x, float y, float dx, float dy)
	:Cf3MapObjectIceBase(MOT_ICE)
{
	m_IceList.insert(this);
	SetPos(x,y);
	m_DX = dx; m_DY = dy;
	m_Life=LIFE;
}

Cf3MapObjectIce::~Cf3MapObjectIce()
{
	m_IceList.erase(this);
}

void Cf3MapObjectIce::OnMove()
{
	if (--m_Life<=0) Kill();
	float Wind=m_pParent->GetWind(floor(m_X/32),floor(m_Y/32));
	m_DX -= (m_DX-Wind)*FRICTION;
	m_DY += GRAVITY;
	m_DY -= m_DY*FRICTION;
	Saturate(-13.0f,m_DX,13.0f);
	Saturate(-13.0f,m_DY,13.0f);
	m_X += m_DX;
	int s=GetSize();
	if (m_DX>0) {
		// 右側当たり判定
		if (m_pParent->GetHit(floor((m_X+s)/32),floor(m_Y/32),HIT_LEFT)) {
			if (floor((m_X+s)/32)!=floor((m_X+s-m_DX)/32)) {
				m_DX*=-REFRECTION;
				m_X = floor((m_X+s)/32)*32-s;
				m_Life--;
			}
		}
	}ef(m_DX<0) {
		// 左側当たり判定
		if (m_pParent->GetHit(floor((m_X-s)/32),floor(m_Y/32),HIT_RIGHT)) {
			if (floor((m_X-s)/32)!=floor((m_X-s-m_DX)/32)) {
				m_DX*=-REFRECTION;
				m_X = floor(m_X/32)*32+s;
				m_Life--;
			}
		}
	}
	m_Y += m_DY;
	if (m_DY>0) {
		// 下側当たり判定
		if (m_pParent->GetHit(floor(m_X/32),floor((m_Y+s)/32),HIT_TOP)) {
			if (floor((m_Y+s)/32)!=floor((m_Y+s-m_DY)/32)) {
				m_DY*=-REFRECTION;
				m_Y = floor((m_Y+s)/32)*32-s;
				m_Life--;
			}
		}
	}ef(m_DY<0) {
		// 上側当たり判定
		if (m_pParent->GetHit(floor(m_X/32),floor((m_Y-s)/32),HIT_BOTTOM)) {
			if (floor((m_Y-s)/32)!=floor((m_Y-s-m_DY)/32)) {
				m_DY*=-REFRECTION;
				m_Y = floor(m_Y/32)*32+s;
				m_Life--;
			}
		}
	}
}

void Cf3MapObjectIce::OnDraw(CDIB32 *lp)
{
	if (!IsValid()||m_Life<=0) return;
	RECT rc;
	rc.left =	64*(15-GetSize());	rc.top =	0;
	rc.right =	64*(16-GetSize());	rc.bottom =	64;
	SetViewPos(-32,-32);
	lp->BltNatural(m_Graphic,m_nVX,m_nVY,&rc);
}

void Cf3MapObjectIce::OnDrawAll(CDIB32 *lp)
{
	int sx, sy, ex, ey;
	sx = sy = 0;
	m_pParent->GetViewPos(sx,sy);
	sx = (-sx)>>5; sy = (-sy)>>5;
	ex = sx+320/32; ey = sy+224/32;
	Saturate(sx,ex,m_pParent->GetWidth()-1);
	Saturate(sy,ey,m_pParent->GetHeight()-1);
	for (Cf3MapObjectBase**it=m_pParent->GetMapObjects(sx-1, sy-1, ex+1, ey+1, MOT_ICE); (*it)!=NULL; it++) {
		if ((*it)->IsValid()) (*it)->OnDraw(lp);
	}
}

void Cf3MapObjectIce::OnMoveAll()
{
	for(set<Cf3MapObjectIce*>::iterator it = m_IceList.begin();it!=m_IceList.end();it++){
		if ((*it)->IsValid()) (*it)->OnMove();
	}
}

void Cf3MapObjectIce::SynergyAll()
{
	for(set<Cf3MapObjectIce*>::iterator it = m_IceList.begin();it!=m_IceList.end();it++){
		if ((*it)->IsValid()) (*it)->Synergy();
	}
}

void Cf3MapObjectIce::OnPreDrawAll()
{
	for(set<Cf3MapObjectIce*>::iterator it = m_IceList.begin();it!=m_IceList.end();it++){
		if ((*it)->IsValid()) (*it)->OnPreDraw();
	}
}

int Cf3MapObjectIce::GetSize()
{
	return 16*m_Life/LIFE;
}
