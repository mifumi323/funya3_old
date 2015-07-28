// f3MapObjectEelPitcher.cpp: Cf3MapObjectEelPitcher �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectEelPitcher.h"
#include "f3MapObjectmrframe.h"
#include "f3MapObjectGeasprin.h"
#include "f3MapObjectNeedle.h"
#include "f3MapObjectMain.h"
#include "f3MapObjectIce.h"
#include "f3MapObjectEffect.h"
#include "f3Map.h"
#include "App.h"
#include "ResourceManager.h"

/*CDIB32 Cf3MapObjectEelPitcher::m_Graphic;
bool Cf3MapObjectEelPitcher::m_bGraphicInitialize = false;*/

set<Cf3MapObjectEelPitcher*>	Cf3MapObjectEelPitcher::m_EnemyList;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

Cf3MapObjectEelPitcher::Cf3MapObjectEelPitcher(int nCX, int nCY)
{
	m_EnemyList.insert(this);
/*	if (!m_bGraphicInitialize) {
		m_Graphic.Load("resource/EelPitcher.gif");
		m_bGraphicInitialize = true;
	}
	m_Graphic.Open("resource/EelPitcher.gif");*/
	m_Graphic = ResourceManager.Get(RID_EELPITCHER);
	m_Delay = 0;
	m_Level = 1;
	SetPos(nCX*32+16,nCY*32+16);
	m_DX = m_DY = 0;
	m_State = EELSEED;
	SetID(OID_EELPITCHER);
	m_bBlinking = false;
}

Cf3MapObjectEelPitcher::~Cf3MapObjectEelPitcher()
{
	m_EnemyList.erase(this);
}

void Cf3MapObjectEelPitcher::OnDraw(CDIB32 *lp)
{
	SetViewPos(-16,-16);
	int height = m_RootY-m_Y;
	RECT rc;
	if (m_State==EELLEAF || m_State==EELFROZEN) {
		int offset1 = (m_State==EELLEAF?0:96);
		int offset2 = (m_Direction==DIR_LEFT?32:64);
		int offset3 = (m_bBlinking?32:0);
		// ������
		int height1 = (height>=16?32:height+16);
		rc.left=offset1+offset2;	rc.top = offset3;
		rc.right=rc.left+32;		rc.bottom = rc.top+height1;
		lp->Blt(m_Graphic,m_nVX,m_nVY,&rc);
		// �s
		if (height>16) {
			int i, h, h2;
			h = floor((m_RootY-m_Y)/(abs(m_RootY-m_Y)+1)+1);
			if (h > 32) h = 32;
			for (i=16; i<height; i+=h) {
				h2 = h<=height-i?h:height-i;
				rc.left=offset1;		rc.top = offset2;
				rc.right=rc.left+32;	rc.bottom = rc.top+h2;
				// +0.5f�͏����_�ȉ��l�̌ܓ��̂���
				SetViewPos(-16+(m_RootX-m_X)*(i-16)/(height-16)+0.5f,-16);
				lp->Blt(m_Graphic,m_nVX,m_nVY+16+i,&rc);
			}
		}
		SetViewPos(-16,-16);
		if (IsIn(17,height,32)) {
			// ����
			int height3 = height-16;
			rc.left=offset1+offset2;	rc.top = 80;
			rc.right=rc.left+32;		rc.bottom = rc.top+height3;
			lp->Blt(m_Graphic,m_nVX,m_nVY+32,&rc);
		}ef(IsIn(33,height,48)) {
			int height2 = height-32;
			// �ӂ���
			rc.left=offset1+offset2;	rc.top = 64;
			rc.right=rc.left+32;		rc.bottom = rc.top+height2;
			lp->Blt(m_Graphic,m_nVX,m_nVY+32,&rc);
			// ����
			rc.left=offset1+offset2;	rc.top = 80;
			rc.right=rc.left+32;		rc.bottom = rc.top+16;
			lp->Blt(m_Graphic,m_nVX,m_nVY+height,&rc);
		}ef(height>48) {
			// �ӂ���
			rc.left=offset1+offset2;	rc.top = 64;
			rc.right=rc.left+32;		rc.bottom = rc.top+16;
			lp->Blt(m_Graphic,m_nVX,m_nVY+32,&rc);
			// ����
			rc.left=offset1+offset2;	rc.top = 80;
			rc.right=rc.left+32;		rc.bottom = rc.top+16;
			lp->Blt(m_Graphic,m_nVX,m_nVY+48,&rc);
		}
	}ef(m_State==EELSEED) {
		rc.left = 0;	rc.top = 0;
		rc.right = 32;	rc.bottom = 32;
		lp->Blt(m_Graphic,m_nVX,m_nVY,&rc);
	}
}

void Cf3MapObjectEelPitcher::OnMove()
{
	if (m_State==EELLEAF){
		BringClose(m_Y,m_RootY-m_Level*32,4.0f);
		m_DX = m_DX + (m_pParent->GetWind(floor(m_X/32),floor(m_Y/32))-m_DX)*m_Level*0.1+(m_RootX-m_X)*0.025;
		Saturate(-14.0f,m_DX,14.0f);
		m_X += m_DX;
		if (m_pParent->GetHit(floor((m_X-16)/32),floor(m_Y/32),HIT_RIGHT)) {
			m_DX = 0;
			m_X = floor(m_X/32)*32+16;
		}ef(m_pParent->GetHit(floor((m_X+16)/32),floor(m_Y/32),HIT_LEFT)) {
			m_DX = 0;
			m_X = floor(m_X/32)*32+16;
		}
	}ef(m_State==EELFROZEN) {
		if (--m_Delay==0) {
			m_Y += 16;
			m_State = EELSEED;
			m_Delay=200;
			new Cf3MapObjectEffect(m_X, m_Y, 0);
		}
	}ef(m_State==EELSEED) {
		BringClose(m_DY,8.0f,1.0f);
		m_DX = m_DX + (m_pParent->GetWind(floor(m_X/32),floor(m_Y/32))-m_DX)*0.2;
		Saturate(-14.0f,m_DX,14.0f);
		m_X += m_DX;
		if (m_pParent->GetHit(floor((m_X-16)/32),floor(m_Y/32),HIT_RIGHT)) {
			m_DX = 0;
			m_X = floor(m_X/32)*32+16;
		}ef(m_pParent->GetHit(floor((m_X+16)/32),floor(m_Y/32),HIT_LEFT)) {
			m_DX = 0;
			m_X = floor(m_X/32)*32+16;
		}
		m_Y += m_DY;
		if (floor(m_Y/32)!=floor((m_Y-m_DY)/32)) {
			// 32�h�b�g���E���܂������I�I
			if (m_pParent->GetHit(floor(m_X/32),floor(m_Y/32),HIT_TOP)
				|| floor(m_Y/32)>=m_pParent->GetHeight()) {
				Seed();
			}else {
				if (m_pParent->GetHit(floor((m_X-16)/32),floor(m_Y/32),HIT_RIGHT)) {
					m_DX = 0;
					m_X = floor(m_X/32)*32+16;
				}ef(m_pParent->GetHit(floor((m_X+16)/32),floor(m_Y/32),HIT_LEFT)) {
					m_DX = 0;
					m_X = floor(m_X/32)*32+16;
				}
			}
		}
	}ef(m_State==EELBUD) {
		if (--m_Delay==0) m_State = EELLEAF;
	}ef(m_State==EELDEAD) {
		Kill();
	}
}

void Cf3MapObjectEelPitcher::Seed()
{
	int d=0;
	m_State = m_Delay?EELBUD:EELLEAF;
	m_RootX = m_X;
	m_RootY = floor(m_Y/32)*32;
	if (m_pParent->GetHit(floor((m_X-14)/32),floor(m_Y/32),HIT_TOP)) d|=1;
	if (m_pParent->GetHit(floor((m_X+14)/32),floor(m_Y/32),HIT_TOP)) d|=2;
	m_Direction = (d==1?DIR_RIGHT:(d==2?DIR_LEFT:((CApp::random(2))?DIR_LEFT:DIR_RIGHT)));
}

void Cf3MapObjectEelPitcher::Synergy()
{
	if (!IsValid()) return;
	m_bBlinking = false;
	if (m_State==EELLEAF) {
		Reaction(m_pParent->GetMainChara());
		for(set<Cf3MapObjectmrframe*>::iterator mf = Cf3MapObjectmrframe::IteratorBegin();
		mf!=Cf3MapObjectmrframe::IteratorEnd();mf++){
			if ((*mf)->IsValid()) Reaction((*mf));
		}
		if (m_RootY-m_Y>16) {
			for(set<Cf3MapObjectIce*>::iterator ic = Cf3MapObjectIce::IteratorBegin();
			ic!=Cf3MapObjectIce::IteratorEnd();ic++){
				if ((*ic)->IsValid()) {
					float objX, objY;
					(*ic)->GetPos(objX,objY);
					if (IsIn(m_X-16,objX,m_X+16)) {
						if (IsIn(m_Y,objY,m_Y+40)) {
							Freeze();
						}
					}
				}
			}
		}
	}
	for(set<Cf3MapObjectEelPitcher*>::iterator ep = Cf3MapObjectEelPitcher::IteratorBegin();
	ep!=Cf3MapObjectEelPitcher::IteratorEnd();ep++){
		if ((*ep)->IsValid()&&(*ep)!=this) {
			float objX, objY;
			(*ep)->GetPos(objX,objY);
			if (m_State==EELLEAF||m_State==EELFROZEN) {
				if (IsIn(m_X-16,objX,m_X+16)) {
					if (IsIn(m_Y,objY,m_Y+16)) {
						if ((*ep)->m_State!=EELLEAF) {
							// �H�ׂ�������I�I
							m_Level+=(*ep)->m_Level;
						}else {
							// �H�ׂ�ꂿ������I�I
							m_State = EELDEAD;
						}
					}
				}
			}ef(m_State==EELSEED) {
				if (IsIn(objX-16,m_X,objX+16)) {
					if (IsIn(objY,m_Y,objY+16)) {
						if ((*ep)->m_State!=EELDEAD) {
							// �H�ׂ�ꂿ������I�I
							m_State = EELDEAD;
						}else {
							// �H�ׂ�������I�I
							m_Level+=(*ep)->m_Level;
						}
					}
				}
			}
		}
	}
	for(set<Cf3MapObjectGeasprin*>::iterator gs = Cf3MapObjectGeasprin::IteratorBegin();
	gs!=Cf3MapObjectGeasprin::IteratorEnd();gs++){
		if ((*gs)->IsValid()) Reaction((*gs));
	}
	for(set<Cf3MapObjectNeedle*>::iterator nd = Cf3MapObjectNeedle::IteratorBegin();
	nd!=Cf3MapObjectNeedle::IteratorEnd();nd++){
		if ((*nd)->IsValid()) Reaction((*nd));
	}
}

void Cf3MapObjectEelPitcher::Reaction(Cf3MapObjectBase *obj)
{
	if (obj==NULL||obj==this) return;
	float objX, objY;
	obj->GetPos(objX,objY);
	switch(obj->GetID()) {
	case OID_FUNYA:{
		if (IsIn(m_X-16,objX,m_X+16)) {
			if (IsIn(m_Y-16,objY,m_Y)) {
				// ���܂ꂽ�I�I
				m_bBlinking = true;
			}
		}else {
		}
		break;
				   }
	case OID_NEEDLE:
	case OID_GEASPRIN:{
		if (IsIn(m_X-16,objX,m_X+16)) {
			if (IsIn(m_Y,objY,m_Y+40)) {
				// �H�ׂ�������I�I
				m_Level++;
			}
		}else {
		}
		break;
				   }
	default:{
		return;
			}
	}
}

void Cf3MapObjectEelPitcher::OnMoveAll()
{
	for(set<Cf3MapObjectEelPitcher*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnMove();
	}
}

void Cf3MapObjectEelPitcher::SynergyAll()
{
	for(set<Cf3MapObjectEelPitcher*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->IsValid())
			(*it)->Synergy();
	}
}

void Cf3MapObjectEelPitcher::OnPreDrawAll()
{
	for(set<Cf3MapObjectEelPitcher*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnPreDraw();
	}
}

void Cf3MapObjectEelPitcher::OnDrawAll(CDIB32 *lp)
{
	for(set<Cf3MapObjectEelPitcher*>::iterator it = m_EnemyList.begin();it!=m_EnemyList.end();it++){
		if ((*it)->m_bValid)
			(*it)->OnDraw(lp);
	}
}

void Cf3MapObjectEelPitcher::Freeze()
{
	m_State = EELFROZEN;
	m_Delay = 80;
}

bool Cf3MapObjectEelPitcher::IsLeaf()
{
	return m_State==EELLEAF || m_State==EELFROZEN;
}