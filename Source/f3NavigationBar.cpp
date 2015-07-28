// f3NavigationBar.cpp: Cf3NavigationBar クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3NavigationBar.h"
#include "ResourceManager.h"

const float NaviSpeed = 0.5f;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3NavigationBar::Cf3NavigationBar()
{
	m_ScrollTo = m_Scroll = 0;
	m_Back = ResourceManager.Get(RID_NAVI);
}

Cf3NavigationBar::~Cf3NavigationBar()
{
}

void Cf3NavigationBar::OnDraw(CDIB32 *lp)
{
	float t=0,o=0;
	list<TNavi*>::iterator it;
	for(it = m_PlaneList.begin();it!=m_PlaneList.end();it++){
		t+=(*it)->rect.right-(*it)->rect.left+(*it)->offset;
	}
	if (t>320) {
		m_ScrollTo+=NaviSpeed;
		if (m_ScrollTo>t) m_ScrollTo-=t;
		if (m_ScrollTo<0) m_ScrollTo+=t;
		if (m_Scroll-m_ScrollTo>t/2) m_Scroll = (m_Scroll+m_ScrollTo-t)/2;
		ef (m_ScrollTo-m_Scroll>t/2) m_Scroll = (m_Scroll+m_ScrollTo+t)/2;
		else m_Scroll = (m_Scroll+m_ScrollTo)/2;
	}else{
		m_ScrollTo = m_Scroll = 0;
	}
	lp->BltFast(m_Back,0,224);
	for(it = m_PlaneList.begin();it!=m_PlaneList.end();it++){
		lp->Blt((*it)->dib,o-m_Scroll,224,&(*it)->rect);
		if (t>320) {
			lp->Blt((*it)->dib,o+t-m_Scroll,224,&(*it)->rect);
			lp->Blt((*it)->dib,o-t-m_Scroll,224,&(*it)->rect);
		}
		o+=(*it)->rect.right-(*it)->rect.left+(*it)->offset;
	}
}

void Cf3NavigationBar::Add(TNavi *lpPlane)
{
	m_PlaneList.remove(lpPlane);
	m_PlaneList.push_back(lpPlane);
	if (lpPlane->rect.bottom==0) {
		int sx, sy;
		lpPlane->dib->GetSize(sx,sy);
		lpPlane->rect.left=0;
		lpPlane->rect.top=0;
		lpPlane->rect.right=sx;
		lpPlane->rect.bottom=sy;
	}
}

void Cf3NavigationBar::Delete(TNavi *lpPlane)
{
	m_PlaneList.remove(lpPlane);
}
