// f3MapObjectBanana.cpp: Cf3MapObjectBanana クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectBanana.h"
#include "f3MapObjectMain.h"
#include "f3MapObjectmrframe.h"
#include "f3NavigationBar.h"
#include "f3Map.h"
#include "ResourceManager.h"

/*CDIB32 Cf3MapObjectBanana::m_Graphic;
bool Cf3MapObjectBanana::m_bGraphicInitialize = false;*/
set<Cf3MapObjectBanana*> Cf3MapObjectBanana::m_BananaList;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3MapObjectBanana::Cf3MapObjectBanana(int nCX, int nCY)
{
	m_BananaList.insert(this);
/*	if (!m_bGraphicInitialize) {
		m_Graphic.Load("resource/Items.gif");
		m_bGraphicInitialize = true;
	}
	m_Graphic.Open("resource/Items.gif");*/
	m_Graphic = ResourceManager.Get(RID_MAIN);
	SetPos(nCX*32+16,nCY*32+16);
	SetID(OID_BANANA);
}

Cf3MapObjectBanana::~Cf3MapObjectBanana()
{
	m_BananaList.erase(this);
}

void Cf3MapObjectBanana::OnDraw(CDIB32 *lp)
{
	if (!IsValid()) return;
	RECT rc = { 320, 96, 352, 128, };
	SetViewPos(-16,-16);
	lp->BltNatural(m_Graphic,m_nVX,m_nVY,&rc);
}

void Cf3MapObjectBanana::Synergy()
{
	if (!IsValid()) return;
	Reaction(m_pParent->GetMainChara());
	for(set<Cf3MapObjectmrframe*>::iterator it = Cf3MapObjectmrframe::IteratorBegin();it!=Cf3MapObjectmrframe::IteratorEnd();it++){
		if ((*it)->IsValid())
			Reaction((*it));
	}
}

void Cf3MapObjectBanana::Reaction(Cf3MapObjectBase *obj)
{
	if (!IsValid()) return;
	switch(obj->GetID()) {
	case OID_FUNYA:{
		int cx1,cy1,cx2,cy2;
		GetCPos(cx1,cy1);
		obj->GetCPos(cx2,cy2);
		if (cx1==cx2&&cy1==cy2) {
			m_pParent->m_nGotBanana++;
			Kill();
		}
		break;
				   }
	default:{
		return;
			}
	}
}

void Cf3MapObjectBanana::OnDrawAll(CDIB32 *lp)
{
	for(set<Cf3MapObjectBanana*>::iterator it = m_BananaList.begin();it!=m_BananaList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnDraw(lp);
	}
}

void Cf3MapObjectBanana::SynergyAll()
{
	for(set<Cf3MapObjectBanana*>::iterator it = m_BananaList.begin();it!=m_BananaList.end();it++){
		if ((*it)->IsValid())
			(*it)->Synergy();
	}
}

void Cf3MapObjectBanana::OnPreDrawAll()
{
	// 今のところOnPreDrawは使われていない
/*	for(set<Cf3MapObjectBanana*>::iterator it = m_BananaList.begin();it!=m_BananaList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnPreDraw();
	}*/
}
