// f3MapObjectBase.cpp: Cf3MapObjectBase �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectBase.h"
#include "f3Map.h"

Cf3Map* Cf3MapObjectBase::m_pParent = NULL;
set<Cf3MapObjectBase*> Cf3MapObjectBase::m_CharaList;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

Cf3MapObjectBase::Cf3MapObjectBase()
{
	m_CharaList.insert(this);
	m_bValid = true;
	m_X = m_Y = 0;
	m_nScrollX = m_nScrollY = 1.0f;	// �W���ŃX�N���[���Ɋ��S�ɂ��Ă䂭
	m_nID = 0;
}

Cf3MapObjectBase::~Cf3MapObjectBase()
{
	m_CharaList.erase(this);
}

void Cf3MapObjectBase::SetViewPos(float offsetx, float offsety)
{
	m_nVX = m_X+offsetx;	m_nVY = m_Y+offsety;
	if (m_pParent!=NULL) m_pParent->GetViewPos(m_nVX,m_nVY,m_nScrollX,m_nScrollY);
}

void Cf3MapObjectBase::SetParent(Cf3Map *lp)
{
	m_pParent = lp;
}

float Cf3MapObjectBase::GetDistance(Cf3MapObjectBase &obj)
{
	return sqrt(GetDistanceSquare(obj));
}

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
	for(set<Cf3MapObjectBase*>::iterator it = m_CharaList.begin();it!=m_CharaList.end();){
		if (!(*it)->IsValid()) {
			Cf3MapObjectBase* lp = *it;
			it++;
			DELETE_SAFE(lp);	//	�C�e���[�^��erase����Ƃ��ɂ��̗v�f��
								//	�w���Ă���ƕs���ɂȂ�
		} else {
			it++;
		}
	}
}

void Cf3MapObjectBase::KillAll()
{
	for(set<Cf3MapObjectBase*>::iterator it = m_CharaList.begin();it!=m_CharaList.end();it++){
		if ((*it)->m_bValid)
			(*it)->Kill();
	}
}
