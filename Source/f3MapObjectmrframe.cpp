// f3MapObjectmrframe.cpp: Cf3MapObjectmrframe クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectmrframe.h"
#include "f3MapObjectEffect.h"
#include "f3Map.h"
#include "yaneSDK/yaneDIB32.h"
#include "yaneSDK/yaneTextDIB32.h"
#include "ResourceManager.h"

set<Cf3MapObjectmrframe*>	Cf3MapObjectmrframe::m_EnemyList;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3MapObjectmrframe::Cf3MapObjectmrframe(int nCX, int nCY)
{
	m_EnemyList.insert(this);
	m_funya = new Cf3MapObjectfunya(nCX,nCY);
	m_CharaList.erase(m_funya);
	m_funya->m_bOriginal = false;
	m_Graphic = ResourceManager.Get(RID_MRFRAME);
	SetID(OID_FUNYA);
	SetPos(m_funya->m_X,m_funya->m_Y);
	m_nLife=100;
}

Cf3MapObjectmrframe::~Cf3MapObjectmrframe()
{
	m_EnemyList.erase(this);
	delete m_funya;
}

void Cf3MapObjectmrframe::OnDraw(CDIB32 *lp)
{
	if (!m_bValid) return;
	if (m_pParent->ItemCompleted()) m_funya->Smile();
	int CX=0, CY=m_funya->m_Direction;
	SetViewPos(-16,-15);
	if (m_funya->m_State==Cf3MapObjectfunya::STANDING) {	// 立ってるとき
	}ef(m_funya->m_State==Cf3MapObjectfunya::RUNNING) {
		CX = m_funya->m_PoseCounter<6?m_funya->m_PoseCounter+2:14-m_funya->m_PoseCounter;
	}ef(m_funya->m_State==Cf3MapObjectfunya::WALKING) {
		CX = 11;
	}ef(m_funya->m_State==Cf3MapObjectfunya::CHARGING) {
		CX=	
		(m_funya->m_ChargePower>=m_funya->m_JumpFunc[0].Power?24:
		(m_funya->m_ChargePower>=m_funya->m_JumpFunc[1].Power?11:
		(m_funya->m_ChargePower>=m_funya->m_JumpFunc[2].Power?25:
		(m_funya->m_ChargePower>=m_funya->m_JumpFunc[3].Power?12:
		12))));
	}ef(m_funya->m_State==Cf3MapObjectfunya::JUMPING) {
		CX = ((m_funya->m_DY>=0)?10:9);
	}ef(m_funya->m_State==Cf3MapObjectfunya::BREATHEIN) {
		if (m_funya->m_ChargePower<40.0f)	{ CX = 15; }
		ef (m_funya->m_ChargePower<120.0f)	{ CX = 16; }
		else						{ CX = 17; }
		if (!m_funya->m_HitBottom) CX += 12;
	}ef(m_funya->m_State==Cf3MapObjectfunya::BREATHEOUT) {
		CX = 14;
		if (!m_funya->m_HitBottom) CX += 12;
	}ef(m_funya->m_State==Cf3MapObjectfunya::TIRED) {
		CX = ((m_funya->m_PoseCounter+1)%40<20)?21:22;
	}ef(m_funya->m_State==Cf3MapObjectfunya::DAMAGED) {
		CX = 13;
	}ef(m_funya->m_State==Cf3MapObjectfunya::FROZEN) {
		CX = 23;
	}ef(m_funya->m_State==Cf3MapObjectfunya::DEAD) {
		CX = 13;	CY = 0;
	}ef(m_funya->m_State==Cf3MapObjectfunya::SMILING) {
		CX = 18;	CY = 0;
	}ef(m_funya->m_State==Cf3MapObjectfunya::SLEEPING) {
		CX = 19+(int)(m_funya->m_PoseCounter>=20);
		if (m_funya->m_Power<-1.0f/4096.0f) CX += 2;
		CY = 0;
	}ef(m_funya->m_State==Cf3MapObjectfunya::BLINKING) {
		CX = 1;
	}
	RECT rc = {CX*32, CY*32, CX*32+32, CY*32+32,};
	lp->BltNatural(m_Graphic,m_nVX,m_nVY,&rc);
}

void Cf3MapObjectmrframe::OnMove()
{
	m_funya->OnMove();
	SetPos(m_funya->m_X,m_funya->m_Y);
	if (m_funya->IsDied()) {
		if (--m_nLife<=0) {
			Kill();
			m_funya->Kill();
			new Cf3MapObjectEffect(m_X, m_Y, 0);
		}
	}
}

void Cf3MapObjectmrframe::OnPreDraw()
{
	m_funya->OnPreDraw();
}

void Cf3MapObjectmrframe::Synergy()
{
	m_funya->Synergy();
}

void Cf3MapObjectmrframe::OnMoveAll()
{
	for(set<Cf3MapObjectmrframe*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->m_bValid) (*it)->OnMove();
	}
}

void Cf3MapObjectmrframe::SynergyAll()
{
	for(set<Cf3MapObjectmrframe*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->IsValid()) (*it)->Synergy();
	}
}

void Cf3MapObjectmrframe::OnPreDrawAll()
{
	for(set<Cf3MapObjectmrframe*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->m_bValid) (*it)->OnPreDraw();
	}
}

void Cf3MapObjectmrframe::OnDrawAll(CDIB32 *lp)
{
	for(set<Cf3MapObjectmrframe*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->m_bValid) (*it)->OnDraw(lp);
	}
}

bool Cf3MapObjectmrframe::IsFrozen()
{
	return m_funya->IsFrozen();
}

void Cf3MapObjectmrframe::UpdateCPos()
{
	m_funya->UpdateCPos();
}
