// f3MapObjectItem.cpp: Cf3MapObjectItem �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectItem.h"
#include "f3NavigationBar.h"
#include "f3Map.h"

set<Cf3MapObjectItem*> Cf3MapObjectItem::m_ItemList;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

Cf3MapObjectItem::Cf3MapObjectItem()
{
	m_ItemList.insert(this);
}

Cf3MapObjectItem::~Cf3MapObjectItem()
{
	m_ItemList.erase(this);
}

void Cf3MapObjectItem::OnPreDrawAll() {
	for(set<Cf3MapObjectItem*>::iterator it = m_ItemList.begin();it!=m_ItemList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnPreDraw();
	}
}

void Cf3MapObjectItem::OnDrawAll(CDIB32 *lp) {
	for(set<Cf3MapObjectItem*>::iterator it = m_ItemList.begin();it!=m_ItemList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnDraw(lp);
	}
}

void Cf3MapObjectItem::OnMoveAll() {
	for(set<Cf3MapObjectItem*>::iterator it = m_ItemList.begin();it!=m_ItemList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnMove();
	}
}

void Cf3MapObjectItem::KillAll()
{
	for(set<Cf3MapObjectItem*>::iterator it = m_ItemList.begin();it!=m_ItemList.end();it++){
		if ((*it)->m_bValid)
			(*it)->Kill();
	}
}

void Cf3MapObjectItem::Garbage()
{
	for(set<Cf3MapObjectItem*>::iterator it = m_ItemList.begin();it!=m_ItemList.end();){
		if (!(*it)->IsValid()) {
			Cf3MapObjectItem* lp = *it;
			it++;
			DELETE_SAFE(lp);	//	�C�e���[�^��erase����Ƃ��ɂ��̗v�f��
								//	�w���Ă���ƕs���ɂȂ�
		} else {
			it++;
		}
	}
}
