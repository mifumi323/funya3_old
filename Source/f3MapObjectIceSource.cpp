// f3MapObjectIceSource.cpp: Cf3MapObjectIceSource クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectIceSource.h"
#include "App.h"

set<Cf3MapObjectIceSource*> Cf3MapObjectIceSource::m_IceList;

#define PHASEMAX	32

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3MapObjectIceSource::Cf3MapObjectIceSource(int x, int y)
{
	m_IceList.insert(this);
	SetPos(x*32+16,y*32+16);
	SetID(OID_ICESOURCE);
	m_Phase = CApp::random(PHASEMAX);
	m_Size = GetSize();
}

Cf3MapObjectIceSource::~Cf3MapObjectIceSource()
{
	m_IceList.erase(this);
}

void Cf3MapObjectIceSource::OnDraw(CDIB32 *lp)
{
	if (!IsValid()) return;
	RECT rc = { (7-m_Size)*64, 0, (8-m_Size)*64, 64, };
	SetViewPos(-32,-32);
	lp->BltNatural(m_Graphic,m_nVX,m_nVY,&rc);
}

void Cf3MapObjectIceSource::OnPreDraw()
{
	if (CApp::random(40)) { m_Phase++; m_Phase%=PHASEMAX; }
	m_Size = GetSize();
}

void Cf3MapObjectIceSource::OnDrawAll(CDIB32 *lp)
{
	for(set<Cf3MapObjectIceSource*>::iterator it = m_IceList.begin();it!=m_IceList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnDraw(lp);
	}
}

void Cf3MapObjectIceSource::OnPreDrawAll()
{
	for(set<Cf3MapObjectIceSource*>::iterator it = m_IceList.begin();it!=m_IceList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnPreDraw();
	}
}

int Cf3MapObjectIceSource::GetSize()
{
/*	int s = floor(sin((2*3.141592653589793238/PHASEMAX)*m_Phase)*2+6);
	if (s>7) s=7;
	if (s<0) s=0;*/
	int s=abs((PHASEMAX/2)-m_Phase)*6/PHASEMAX+4;
	return s;
}
