// f3MapObjectNeedle.cpp: Cf3MapObjectNeedle クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectNeedle.h"
#include "f3MapObjectMain.h"
#include "f3MapObjectGeasprin.h"
#include "f3MapObjectmrframe.h"
#include "f3MapObjectEelPitcher.h"
#include "f3MapObjectEffect.h"
#include "f3Map.h"
#include "yaneSDK/yaneDIB32.h"
#include "ResourceManager.h"

/*CDIB32 Cf3MapObjectNeedle::m_Graphic;
bool Cf3MapObjectNeedle::m_bGraphicInitialize = false;*/

set<Cf3MapObjectNeedle*> Cf3MapObjectNeedle::m_EnemyList;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3MapObjectNeedle::Cf3MapObjectNeedle(int nCX, int nCY, int nType)
{
	m_EnemyList.insert(this);
	m_Graphic = ResourceManager.Get(RID_NEEDLE);
	SetPos(nCX*32+16,nCY*32+17);
	SetID(OID_NEEDLE);
	switch(nType){
	case 1:
		m_Type = NDT_HORIZONTAL;
		m_State=NDS_LEFT;
		break;
	case 2:
		m_Type = NDT_VERTICAL;
		m_StartY = m_Y;
		m_State=NDS_STOP;
		break;
	case 3:
		m_Type = NDT_HORIZONTAL;
		m_State=NDS_RIGHT;
		break;
	default:
		m_Type=NDT_UNDEFINED;
		break;
	}
	m_Speed = 0;
}

Cf3MapObjectNeedle::~Cf3MapObjectNeedle()
{
	m_EnemyList.erase(this);
}

void Cf3MapObjectNeedle::OnDraw(CDIB32 *lp)
{
	if (!m_bValid) return;
	SetViewPos(-15,-15);
	lp->Blt(m_Graphic,m_nVX,m_nVY);
}

void Cf3MapObjectNeedle::OnMove()
{
	if (!m_bValid) return;
	if (m_Type==NDT_UNDEFINED) {
		// このタイミングで初期化
		if (m_pParent->GetHit(floor(m_X/32),floor((m_Y+16)/32),HIT_TOP)) {
			m_Type = NDT_HORIZONTAL;
			m_State=NDS_STOP;
		}else{
			m_Type = NDT_VERTICAL;
			m_StartY = m_Y;
			m_State=NDS_STOP;
		}
	}
	if (m_Type==NDT_HORIZONTAL) {
		if (m_State==NDS_STOP) {
			BringClose(m_Speed,0.0f,1.0f);
			if (m_Speed==0) {
				if (!m_pParent->GetHit(floor((m_X+15)/32),floor((m_Y+16)/32),HIT_TOP)||
					m_pParent->GetHit(floor((m_X+15)/32),floor((m_Y)/32),HIT_LEFT)) {
					m_State=NDS_LEFT;
				}else{
					m_State=NDS_RIGHT;
				}
			}
		}ef(m_State==NDS_LEFT) {
			m_X-=1;
			if (!m_pParent->GetHit(floor((m_X-16)/32),floor((m_Y+16)/32),HIT_TOP)||
				m_pParent->GetHit(floor((m_X-16)/32),floor((m_Y)/32),HIT_RIGHT)) {
				m_State=NDS_STOP;
				m_Speed=20;
			}
		}ef(m_State==NDS_RIGHT) {
			m_X+=1;
			if (!m_pParent->GetHit(floor((m_X+15)/32),floor((m_Y+16)/32),HIT_TOP)||
				m_pParent->GetHit(floor((m_X+15)/32),floor((m_Y)/32),HIT_LEFT)) {
				m_State=NDS_STOP;
				m_Speed=20;
			}
		}
	}ef(m_Type==NDT_VERTICAL) {
		if (m_State==NDS_STOP) {
			if (m_Speed!=0) {
				BringClose(m_Speed,0.0f,1.0f);
				if (m_Speed==0) {
					m_State=NDS_UP;
				}
			}
		}ef(m_State==NDS_UP) {
			BringClose(m_Y,m_StartY,1.0f);
			if (m_Y==m_StartY) {
				m_State=NDS_STOP;
			}
		}ef(m_State==NDS_DOWN) {
			m_Speed+=0.2f;
			Saturate(0.0f,m_Speed,10.0f);
			m_Y += m_Speed;
			if (m_pParent->GetHit(floor(m_X/32),floor((m_Y+16)/32),HIT_TOP)) {
				m_Y = floor((m_Y+16)/32)*32-15;
				m_Speed = 20;
				m_State=NDS_STOP;
			}ef(m_Y>m_pParent->GetHeight()*32+16) {
				m_Type = NDT_DEAD;
				new Cf3MapObjectEffect(m_X, m_Y, 1);
			}
		}
	}ef(m_Type==NDT_DEAD) {
		Kill();
	}
}

void Cf3MapObjectNeedle::Synergy()
{
	if (!m_bValid) return;
	Reaction(m_pParent->GetMainChara());
	for(set<Cf3MapObjectmrframe*>::iterator mf = Cf3MapObjectmrframe::IteratorBegin();
	mf!=Cf3MapObjectmrframe::IteratorEnd();mf++){
		if ((*mf)->IsValid()) Reaction((*mf));
	}
	for(set<Cf3MapObjectGeasprin*>::iterator gs = Cf3MapObjectGeasprin::IteratorBegin();
	gs!=Cf3MapObjectGeasprin::IteratorEnd();gs++){
		if ((*gs)->IsValid()) Reaction((*gs));
	}
	for(set<Cf3MapObjectEelPitcher*>::iterator ep = Cf3MapObjectEelPitcher::IteratorBegin();
	ep!=Cf3MapObjectEelPitcher::IteratorEnd();ep++){
		if ((*ep)->IsValid()) {
			float objX, objY;
			(*ep)->GetPos(objX,objY);
			if (IsIn(objX-16,m_X,objX+16)) {
				if (IsIn(objY,m_Y,objY+40)) {
					// 食べられちゃった！！
					m_Type = NDT_DEAD;
					new Cf3MapObjectEffect(m_X, m_Y, 0);
				}
			}
		}
	}
}

void Cf3MapObjectNeedle::Reaction(Cf3MapObjectBase *obj)
{
	if (obj==NULL) return;
	float objX, objY;
	obj->GetPos(objX,objY);
	switch(obj->GetID()) {
	case OID_FUNYA:
	case OID_GEASPRIN:{
		if (m_Type!=NDT_VERTICAL || m_State!=NDS_STOP || m_Speed!=0) return;
		if (IsIn(m_X-16,objX,m_X+16)) {
			if (IsIn(m_Y+15,objY,m_Y+271)) {
				m_State = NDS_DOWN;
			}
		}
		break;
				   }
	default:{
		return;
			}
	}
}

void Cf3MapObjectNeedle::OnMoveAll()
{
	for(set<Cf3MapObjectNeedle*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnMove();
	}
}

void Cf3MapObjectNeedle::SynergyAll()
{
	for(set<Cf3MapObjectNeedle*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->IsValid())
			(*it)->Synergy();
	}
}

void Cf3MapObjectNeedle::OnPreDrawAll()
{
	for(set<Cf3MapObjectNeedle*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnPreDraw();
	}
}

void Cf3MapObjectNeedle::OnDrawAll(CDIB32 *lp)
{
	for(set<Cf3MapObjectNeedle*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnDraw(lp);
	}
}
