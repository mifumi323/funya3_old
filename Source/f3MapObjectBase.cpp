// f3MapObjectBase.cpp: Cf3MapObjectBase クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectBase.h"
#include "f3Map.h"

int Cf3MapObjectBase::m_nNextID = 0;
Cf3Map* Cf3MapObjectBase::m_pParent = NULL;
MapObjectList Cf3MapObjectBase::m_CharaList;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3MapObjectBase::Cf3MapObjectBase(f3MapObjectType eType)
	:m_bValid(true)
	,m_eType(eType)
	,m_X(0) ,m_Y(0)
	,m_nCX(-1) ,m_nCY(-1)
	,m_nID(m_nNextID++)
	,m_pNext(NULL)
{
	m_CharaList.insert(this);
//	m_nScrollX = m_nScrollY = 1.0f;	// 標準でスクロールに完全についてゆく
}

Cf3MapObjectBase::~Cf3MapObjectBase()
{
	m_pParent->RemoveMapObject(m_nCX, m_nCY, this);
	m_CharaList.erase(this);
}

void Cf3MapObjectBase::Kill()
{
	m_pParent->RemoveMapObject(m_nCX, m_nCY, this);
	m_bValid = false;
}

void Cf3MapObjectBase::SetViewPos(float offsetx, float offsety)
{
	m_nVX = m_X+offsetx;
	m_nVY = m_Y+offsety;
	if (m_pParent!=NULL) m_pParent->GetViewPos(m_nVX, m_nVY);
}

//float Cf3MapObjectBase::GetDistance(Cf3MapObjectBase &obj)
//{
//	return sqrt(GetDistanceSquare(obj));
//}

float Cf3MapObjectBase::GetDistanceSquare(Cf3MapObjectBase& obj)
{
	float x,y;
	obj.GetPos(x,y);
	return (x-m_X)*(x-m_X)+(y-m_Y)*(y-m_Y);
}

void Cf3MapObjectBase::GetCPos(int &x, int &y)
{
	x = floor(m_X/32); y = floor(m_Y/32);
}

void Cf3MapObjectBase::Garbage()
{
	for(MapObjectList::iterator it = m_CharaList.begin();it!=m_CharaList.end();){
		if (!(*it)->IsValid()) {
			Cf3MapObjectBase* lp = *it;
			it++;
			DELETE_SAFE(lp);	//	イテレータはeraseするときにその要素を
								//	指していると不正になる
		} else {
			it++;
		}
	}
}

void Cf3MapObjectBase::KillAll()
{
	for(MapObjectList::iterator it = m_CharaList.begin();it!=m_CharaList.end();it++){
		if ((*it)->m_bValid) (*it)->Kill();
	}
}

void Cf3MapObjectBase::UpdateCPos()
{
	int cx, cy;
	GetCPos(cx, cy);
	if (m_nCX!=cx||m_nCY!=cy) {
		m_pParent->RemoveMapObject(m_nCX, m_nCY, this);
		m_pParent->AddMapObject(m_nCX=cx, m_nCY=cy, this);
	}
}

void Cf3MapObjectBase::UpdateCPosAll()
{
	for(MapObjectList::iterator it = m_CharaList.begin();it!=m_CharaList.end();it++){
		if ((*it)->m_bValid) (*it)->UpdateCPos();
	}
}
