// f3MapObjectGeasprin.cpp: Cf3MapObjectGeasprin クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectGeasprin.h"
#include "f3MapObjectEelPitcher.h"
#include "f3MapObjectmrframe.h"
#include "f3MapObjectIce.h"
#include "f3MapObjectMain.h"
#include "f3MapObjectEffect.h"
#include "f3Map.h"
#include "App.h"
#include "yaneSDK/yaneDIB32.h"
#include "ResourceManager.h"

/*CDIB32 Cf3MapObjectGeasprin::m_Graphic;
bool Cf3MapObjectGeasprin::m_bGraphicInitialize = false;*/

set<Cf3MapObjectGeasprin*> Cf3MapObjectGeasprin::m_EnemyList;

const WalkDelay = 10;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3MapObjectGeasprin::Cf3MapObjectGeasprin(int nCX, int nCY, f3MapObjectDirection direction)
{
	m_EnemyList.insert(this);
/*	if (!m_bGraphicInitialize) {
		m_Graphic.Load("resource/Geasprin.gif");
		m_bGraphicInitialize = true;
	}
	m_Graphic.Open("resource/Geasprin.gif");*/
	m_Graphic = ResourceManager.Get(RID_GEASPRIN);
	SetPos(nCX*32+16,nCY*32+16);
	SetID(OID_GEASPRIN);
	m_Direction = direction;
	m_Spring[DIR_FRONT] = m_Spring[DIR_LEFT] = m_Spring[DIR_RIGHT] = m_Spring2[DIR_FRONT] = m_Spring2[DIR_LEFT] = m_Spring2[DIR_RIGHT] = 0;
	Stop();
}

Cf3MapObjectGeasprin::~Cf3MapObjectGeasprin()
{
	m_EnemyList.erase(this);
}

void Cf3MapObjectGeasprin::OnDraw(CDIB32 *lp)
{
	if (!m_bValid) return;
	SetViewPos(-16,-16);
	// バネ
	if (m_Spring2[DIR_FRONT]) {
		int h = m_Spring2[DIR_FRONT];
		RECT rc = {32, 64, 64, 64+h,};
		lp->BltNatural(m_Graphic,m_nVX,m_nVY+8-h,&rc);
	}
	if (m_Spring2[DIR_LEFT]) {
		int w = m_Spring2[DIR_LEFT];
		RECT rc = {0, 64, w, 96,};
		lp->BltNatural(m_Graphic,m_nVX+8-w,m_nVY,&rc);
	}
	if (m_Spring2[DIR_RIGHT]) {
		int w = m_Spring2[DIR_RIGHT];
		RECT rc = {32-w, 64, 32, 96,};
		lp->BltNatural(m_Graphic,m_nVX+24,m_nVY,&rc);
	}
	// 本体
	int CX=0, CY=(m_Direction==DIR_LEFT?0:1);
	if (m_State==STANDING) {
	}ef(m_State==WALKING) {
		CX = 1+(m_GX&1);
	}ef(m_State==FALLING) {
		CX = 5;
	}ef(m_State==LAUGHING) {
		if (IsIn(20,m_Delay,40)) {
			CX = 3+(m_Delay/4&1);
		}ef(IsIn(0,m_Delay,20)) {
			CX = 4;
		}
	}ef(m_State==BLOWN) {
		CX = 6;
	}ef(m_State==FROZEN) {
		CX = ((m_Delay<40&&(m_Delay>>1)&1)?6:7);
	}
	RECT rc = {CX*32, CY*32, CX*32+32, CY*32+32,};
	lp->BltNatural(m_Graphic,m_nVX,m_nVY,&rc);
}

void Cf3MapObjectGeasprin::OnMove()
{
	if (!m_bValid) return;
	if (m_Delay) m_Delay--;
	if (m_State==WALKING) {
		if (m_Direction==DIR_LEFT&&m_pParent->GetHit(floor((m_X-17)/32),floor(m_Y/32),HIT_RIGHT)){
			Blow();
		}ef (m_Direction==DIR_RIGHT&&m_pParent->GetHit(floor((m_X+17)/32),floor(m_Y/32),HIT_LEFT)){
			Blow();
		}ef(!m_pParent->GetHit(floor((m_X+17*(m_Direction==DIR_LEFT?-1:1))/32),floor((m_Y+17)/32),HIT_TOP)){
			Stop();
		}else {
			if (m_Delay==0) {
				m_Delay = WalkDelay;
				m_GX += (m_Direction==DIR_LEFT?-1:1);
			}
		}
	}ef (m_State==STANDING) {
		if (!m_pParent->GetHit(floor(m_X/32),floor((m_Y+17)/32),HIT_TOP)){
			Stop();
		}else {
			if (m_Delay==0) {
				Walk();
			}
		}
	}ef (m_State==FALLING) {
		if (m_Delay==0) {
			m_DY += 2;
			Saturate(-60,m_DY,60);
			m_GY += m_DY;
		}
		m_X = m_GX*8; m_Y = m_GY/8;
		if (m_DY>0&&m_pParent->GetHit(floor(m_X/32),floor((m_Y+17)/32),HIT_TOP)){
			m_GY=((floor((m_Y+17)/32)-1)*32+16)*8;
			Laugh();
		}ef(m_DY<0&&m_pParent->GetHit(floor(m_X/32),floor((m_Y-15)/32),HIT_BOTTOM)){
			m_GY=(m_GY+127)&~127;
			m_DY=0;
		}
	}ef (m_State==LAUGHING) {
		if (m_Delay==0) {
			Stop();
		}
	}ef (m_State==BLOWN) {
		if (m_Direction==DIR_LEFT&&!m_pParent->GetHit(floor((m_X+17)/32),floor(m_Y/32),HIT_LEFT)){
			m_GX++;
		}ef (m_Direction==DIR_RIGHT&&!m_pParent->GetHit(floor((m_X-17)/32),floor(m_Y/32),HIT_RIGHT)){
			m_GX--;
		}
		if (m_Delay==0) {
			Stop();
		}
	}ef (m_State==BACK) {
		if (!m_pParent->GetHit(floor((m_X+17*(m_Direction!=DIR_LEFT?-1:1))/32),floor((m_Y+17)/32),HIT_TOP)){
		}ef(m_Direction==DIR_LEFT&&!m_pParent->GetHit(floor((m_X+17)/32),floor(m_Y/32),HIT_LEFT)){
			m_GX++;
		}ef(m_Direction==DIR_RIGHT&&!m_pParent->GetHit(floor((m_X-17)/32),floor(m_Y/32),HIT_RIGHT)){
			m_GX--;
		}
		if (m_Delay==0) {
			Laugh();
		}
	}ef (m_State==FROZEN) {
		if (m_Delay==0) {
			Stop();
		}
	}ef (m_State==DEAD) {
		Kill();
	}
	if (m_Spring[DIR_FRONT]) m_Spring2[DIR_FRONT] = CApp::random(m_Spring[DIR_FRONT]);
	if (m_Spring[DIR_LEFT]) m_Spring2[DIR_LEFT] = CApp::random(m_Spring[DIR_LEFT]);
	if (m_Spring[DIR_RIGHT]) m_Spring2[DIR_RIGHT] = CApp::random(m_Spring[DIR_RIGHT]);
	m_X = m_GX*8; m_Y = m_GY/8;
	if (m_Y>m_pParent->GetHeight()*32+16) {
		Kill();
		new Cf3MapObjectEffect(m_X, m_Y, 1);
	}
}

void Cf3MapObjectGeasprin::SetPos(float x, float y)
{
	m_X = x; m_Y = y;
	m_GX = floor(x/8); m_GY = floor(y*8);
}

void Cf3MapObjectGeasprin::Walk()
{
	m_Delay = WalkDelay;
	m_State = WALKING;
	m_Direction = (m_Direction==DIR_LEFT?DIR_RIGHT:DIR_LEFT);
}

void Cf3MapObjectGeasprin::Stop()
{
	if (m_pParent->GetHit(floor(m_X/32),floor((m_Y+17)/32),HIT_TOP)) {
		m_Delay = 40;
		m_State = STANDING;
	}else {
		m_Delay = 40;
		m_DY = 0;
		m_State = FALLING;
	}
}

void Cf3MapObjectGeasprin::Laugh()
{
	m_Delay = 80;
	m_State = LAUGHING;
}

void Cf3MapObjectGeasprin::Blow(f3MapObjectDirection dir)
{
	if (dir!=DIR_FRONT) m_Direction = dir;
	m_Spring[m_Direction] = 32;
	m_Delay = 20;
	m_State=BLOWN;
}

void Cf3MapObjectGeasprin::Synergy()
{
	if (!IsValid()) return;
	Cf3MapObjectBase**it;
	// ウナギカズラ
	for(it=m_pParent->GetMapObjects(m_nCX-2, m_nCY-2, m_nCX+2, m_nCY+2, OID_EELPITCHER); (*it)!=NULL; it++){
		if ((*it)->IsValid()) {
			float objX, objY;
			(*it)->GetPos(objX,objY);
			if (IsIn(objX-16,m_X,objX+16)) {
				if (IsIn(objY,m_Y,objY+40)) {
					// 食べられちゃった！！
					m_State = DEAD;
					new Cf3MapObjectEffect(m_X, m_Y, 0);
					return;
				}
			}
		}
	}
	if (!IsFrozen()) {
		// ふにゃ
		for(it=m_pParent->GetMapObjects(m_nCX-2, m_nCY-2, m_nCX+2, m_nCY+2, OID_FUNYA); (*it)!=NULL; it++){
			if ((*it)->IsValid()) Reaction((*it));
		}
		// ギヤバネ
		for(it=m_pParent->GetMapObjects(m_nCX-2, m_nCY-2, m_nCX+2, m_nCY+2, OID_GEASPRIN); (*it)!=NULL; it++){
			if ((*it)!=this&&(*it)->IsValid()) {
				float objX, objY;
				(*it)->GetPos(objX,objY);
				if (IsIn(m_X-8,objX,m_X+8)) {
					if (IsIn(m_Y-32,objY,m_Y)) {
						// 踏まれた！
						Laugh();
						m_Spring[DIR_FRONT] = 32;
					}ef(IsIn(m_Y,objY,m_Y+32)) {
						// 踏んだ！
						if (((Cf3MapObjectGeasprin*)(*it))->m_State!=FROZEN) {
							Jump();
						}else {
							m_Y = objY-32.125f;
							m_GY = m_Y*8;
							Freeze();
						}
					}
				}ef(IsIn(m_X+16,objX,m_X+29)) {
					if (IsIn(m_Y-16,objY,m_Y+16)) {
						// 右から来た！
						if (((Cf3MapObjectGeasprin*)(*it))->m_State!=FROZEN) {
							Blow(DIR_RIGHT);
							m_Spring[DIR_RIGHT] = 32;
						}else {
							m_X = objX-40;
							m_GX = m_X/8;
							Freeze();
						}
					}
				}ef(IsIn(m_X-29,objX,m_X-16)) {
					if (IsIn(m_Y-16,objY,m_Y+16)) {
						// 左から来た！
						if (((Cf3MapObjectGeasprin*)(*it))->m_State!=FROZEN) {
							Blow(DIR_LEFT);
							m_Spring[DIR_LEFT] = 32;
						}else {
							m_X = objX+40;
							m_GX = m_X/8;
							Freeze();
						}
					}
				}
			}
		}
		// 氷
		for(it=m_pParent->GetMapObjects(m_nCX-2, m_nCY-2, m_nCX+2, m_nCY+2, OID_ICE); (*it)!=NULL; it++){
			if ((*it)->IsValid()) {
				float objX, objY;
				(*it)->GetPos(objX,objY);
				if ((objX-m_X)*(objX-m_X)+(objY-m_Y)*(objY-m_Y)<256) {
					// あたった！
					Freeze();
				}
			}
		}
	}
}

void Cf3MapObjectGeasprin::Reaction(Cf3MapObjectBase *obj)
{
	if (obj==NULL) return;
	float objX, objY;
	obj->GetPos(objX,objY);
	switch(obj->GetID()) {
	case OID_FUNYA:{
		if (IsIn(m_X-16,objX,m_X+16)) {
			if (IsIn(m_Y-32,objY,m_Y)) {
				// 踏まれた！
				Laugh();
				m_Spring[DIR_FRONT] = 32;
				if (!m_pParent->GetHit(floor(m_X/32),floor((m_Y+16)/32),HIT_TOP)) m_GY++;
			}
		}ef(IsIn(m_X+16,objX,m_X+29)) {
			if (IsIn(m_Y-16,objY,m_Y+16)) {
				// 右から来た！
				if (m_pParent->GetHit(floor(m_X/32),floor((m_Y+16)/32),HIT_TOP)) Back(DIR_RIGHT);
				else Laugh();
				m_Spring[DIR_RIGHT] = 32;
			}
		}ef(IsIn(m_X-29,objX,m_X-16)) {
			if (IsIn(m_Y-16,objY,m_Y+16)) {
				// 左から来た！
				if (m_pParent->GetHit(floor(m_X/32),floor((m_Y+16)/32),HIT_TOP)) Back(DIR_LEFT);
				else Laugh();
				m_Spring[DIR_LEFT] = 32;
			}
		}
		break;
				   }
	default:{
		return;
			}
	}
}

void Cf3MapObjectGeasprin::Jump()
{
	m_DY = -60;
	m_State = FALLING;
}

void Cf3MapObjectGeasprin::OnMoveAll()
{
	for(set<Cf3MapObjectGeasprin*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->m_bValid) (*it)->OnMove();
	}
}

void Cf3MapObjectGeasprin::SynergyAll()
{
	for(set<Cf3MapObjectGeasprin*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->IsValid()) (*it)->Synergy();
	}
}

void Cf3MapObjectGeasprin::OnPreDrawAll()
{
	for(set<Cf3MapObjectGeasprin*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->m_bValid) (*it)->OnPreDraw();
	}
}

void Cf3MapObjectGeasprin::OnDrawAll(CDIB32 *lp)
{
	int sx, sy, ex, ey;
	sx = sy = 0;
	m_pParent->GetViewPos(sx,sy);
	sx = (-sx)>>5; sy = (-sy)>>5;
	ex = sx+320/32; ey = sy+224/32;
	Saturate(sx,ex,m_pParent->GetWidth()-1);
	Saturate(sy,ey,m_pParent->GetHeight()-1);
	for (Cf3MapObjectBase**it=m_pParent->GetMapObjects(sx-1, sy-1, ex+1, ey+1, OID_GEASPRIN); (*it)!=NULL; it++) {
		if ((*it)->IsValid()) (*it)->OnDraw(lp);
	}
/*	for(set<Cf3MapObjectGeasprin*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->m_bValid) (*it)->OnDraw(lp);
	}*/
}

void Cf3MapObjectGeasprin::OnPreDraw()
{
	if (m_Spring[DIR_FRONT]) {
		if (m_Spring[DIR_FRONT]>32) m_Spring[DIR_FRONT] = 32;
		m_Spring[DIR_FRONT]--;
	}
	if (m_Spring[DIR_LEFT]) {
		if (m_Spring[DIR_LEFT]>32) m_Spring[DIR_LEFT] = 32;
		m_Spring[DIR_LEFT]--;
	}
	if (m_Spring[DIR_RIGHT]) {
		if (m_Spring[DIR_RIGHT]>32) m_Spring[DIR_RIGHT] = 32;
		m_Spring[DIR_RIGHT]--;
	}
}

void Cf3MapObjectGeasprin::Back(f3MapObjectDirection dir)
{
	if (dir!=DIR_FRONT) m_Direction = dir;
	m_Delay = 1;
	m_State=BACK;
}

void Cf3MapObjectGeasprin::Freeze()
{
	m_Delay = 200;
	m_State=FROZEN;
}
