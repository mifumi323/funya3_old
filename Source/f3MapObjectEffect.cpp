// f3MapObjectEffect.cpp: Cf3MapObjectEffect クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectEffect.h"
#include "f3Map.h"
#include "App.h"
#include "ResourceManager.h"

set<Cf3MapObjectEffect*> Cf3MapObjectEffect::m_EffectList;
/*CDIB32 Cf3MapObjectEffect::m_Graphic;
bool Cf3MapObjectEffect::m_bGraphicInitialize = false;*/
RECT Cf3MapObjectEffect::m_GraphicRect[4*16] = {
	{ 0,0, 5,5}, { 0,5, 5,10}, { 0,10, 5,15}, { 0,15, 5,20},
	{ 5,0,10,5}, { 5,5,10,10}, { 5,10,10,15}, { 5,15,10,20},
	{10,0,15,5}, {10,5,15,10}, {10,10,15,15}, {10,15,15,20},
	{15,0,20,5}, {15,5,20,10}, {15,10,20,15}, {15,15,20,20},
	{20,0,25,5}, {20,5,25,10}, {20,10,25,15}, {20,15,25,20},
	{25,0,30,5}, {25,5,30,10}, {25,10,30,15}, {25,15,30,20},
	{30,0,35,5}, {30,5,35,10}, {30,10,35,15}, {30,15,35,20},
	{35,0,40,5}, {35,5,40,10}, {35,10,40,15}, {35,15,40,20},
	{40,0,45,5}, {40,5,45,10}, {40,10,45,15}, {40,15,45,20},
	{45,0,50,5}, {45,5,50,10}, {45,10,50,15}, {45,15,50,20},
	{50,0,55,5}, {50,5,55,10}, {50,10,55,15}, {50,15,55,20},
	{55,0,60,5}, {55,5,60,10}, {55,10,60,15}, {55,15,60,20},
	{60,0,65,5}, {60,5,65,10}, {60,10,65,15}, {60,15,65,20},
	{65,0,70,5}, {65,5,70,10}, {65,10,70,15}, {65,15,70,20},
	{70,0,75,5}, {70,5,75,10}, {70,10,75,15}, {70,15,75,20},
	{75,0,80,5}, {75,5,80,10}, {75,10,80,15}, {75,15,80,20},
};

const float PI=3.141592653589793238f;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3MapObjectEffect::Cf3MapObjectEffect(float x, float y, int EffectType)
{
	m_EffectList.insert(this);
	m_Graphic = ResourceManager.Get(RID_EFFECT);
	SetPos(x,y);
	m_StarNum = 0;
	m_Star = NULL;
	m_nEffectType = EffectType;
	if (EffectType==0) {
		m_StarNum = 12;
		m_Star = new tagStar[m_StarNum];
		for (int i=0; i<m_StarNum; i++) {
			float rad=2.0*PI*i/m_StarNum;
			m_Star[i].dx=4.0f*cos(rad)*(0.5+0.5/4096.0*CApp::random(4096));
			m_Star[i].dy=4.0f*sin(rad)*(0.5+0.5/4096.0*CApp::random(4096));
			m_Star[i].x =0;
			m_Star[i].y =0;
			m_Star[i].f =0.9f;
			m_Star[i].n =40;
		}
	}ef(EffectType==1) {
		m_StarNum = 12;
		m_Star = new tagStar[m_StarNum];
		for (int i=0; i<m_StarNum; i++) {
			float rad=2.0*PI*i/m_StarNum;
			m_Star[i].dx=0;
			m_Star[i].dy=-16.0f*(0.5+0.5/4096.0*CApp::random(4096));
			m_Star[i].x =32*(-0.5+1.0/4096.0*CApp::random(4096));
			m_Star[i].y =0;
			m_Star[i].f =0.9f;
			m_Star[i].n =35+CApp::random(10);
		}
	}else{
		m_bValid = false;
		return;
	}
	for (int i=0; i<m_StarNum; i++) {
		m_Star[i].r =CApp::random(4*16);
	}
	SetID(OID_EFFECT);
}

Cf3MapObjectEffect::~Cf3MapObjectEffect()
{
	m_EffectList.erase(this);
	DELETEPTR_SAFE(m_Star);
}

void Cf3MapObjectEffect::OnPreDrawAll() {
	for(set<Cf3MapObjectEffect*>::iterator it = m_EffectList.begin();it!=m_EffectList.end();it++){
		if ((*it)->m_bValid) (*it)->OnPreDraw();
	}
}

void Cf3MapObjectEffect::OnDrawAll(CDIB32 *lp) {
	int sx, sy, ex, ey;
	sx = sy = 0;
	m_pParent->GetViewPos(sx,sy);
	sx = (-sx)>>5; sy = (-sy)>>5;
	ex = sx+320/32; ey = sy+224/32;
	Saturate(sx,ex,m_pParent->GetWidth()-1);
	Saturate(sy,ey,m_pParent->GetHeight()-1);
	for (Cf3MapObjectBase**it=m_pParent->GetMapObjects(sx-3, sy-3, ex+3, ey+3, OID_EFFECT); (*it)!=NULL; it++) {
		if ((*it)->IsValid()) (*it)->OnDraw(lp);
	}
/*	for(set<Cf3MapObjectEffect*>::iterator it = m_EffectList.begin();it!=m_EffectList.end();it++){
		if ((*it)->m_bValid) (*it)->OnDraw(lp);
	}*/
}

void Cf3MapObjectEffect::OnPreDraw()
{
	int n=m_StarNum;
	for (int i=0; i<m_StarNum; i++) {
		if (m_Star[i].n) m_Star[i].n--; else n--;
		m_Star[i].dx*=m_Star[i].f;
		m_Star[i].dy*=m_Star[i].f;
		m_Star[i].x +=m_Star[i].dx;
		m_Star[i].y +=m_Star[i].dy;
	}
	if (!n) Kill();
}

void Cf3MapObjectEffect::OnDraw(CDIB32 *lp)
{
	for (int i=0; i<m_StarNum; i++) {
		if (m_Star[i].n) {
			SetViewPos(m_Star[i].x,m_Star[i].y);
			lp->Blt(m_Graphic,m_nVX,m_nVY,&m_GraphicRect[m_Star[i].r]);
		}
	}
}
