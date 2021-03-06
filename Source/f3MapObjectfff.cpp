// f3MapObjectfff.cpp: Cf3MapObjectfff クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectfff.h"
#include "f3MapObjectGeasprin.h"
#include "f3MapObjectNeedle.h"
#include "f3MapObjectEelPitcher.h"
#include "f3MapObjectIce.h"
#include "f3MapObjectIceSource.h"
#include "f3MapObjectFire.h"
#include "f3MapObjectBanana.h"
#include "f3Map.h"
#include "ResourceManager.h"
#include "yaneSDK/yaneSinTable.h"

const float FLYACCEL = 0.24f;
const float FLYFRICTION = 0.026f;
const float ROTATEACCEL = 0.5f;
const float ROTATEFRICTION = 0.01f;

Cf3MapObjectfff::Cf3MapObjectfff(int nCX, int nCY)
	:Cf3MapObjectMain(MOT_FUNYA)
	,m_DX   (0.0f), m_DY   (0.0f)
	,m_OldDX(0.0f), m_OldDY(0.0f)
	,m_Angle(0.0f), m_DAngle(0.0f)
	,m_nPower(0)
	,m_Power(0.0f)
	,m_PowerX(0.0f), m_PowerY(0.0f)
	,m_VOffsetX  (0), m_VOffsetY  (0)
	,m_VOffsetToX(0), m_VOffsetToY(0)
	,m_PoseCounter2(0)
	,m_State(NORMAL)
{
//	m_Graphic = ResourceManager.Get(RID_MAIN);
//	m_Graphic2 = ResourceManager.Get(RID_MAINICY);
	SetPos(nCX*32+16,nCY*32+18);
	m_OldX = m_X; m_OldY = m_Y;
}

void Cf3MapObjectfff::OnDraw(CDIB32 *lp)
{
	if (!IsValid()) return;
	if (m_pParent->ItemCompleted()) Smile();
	int CX=0, CY=0;
	SetViewPos(-15,-15);
	switch (m_State) {
	case NORMAL:
		CX=m_pInput->GetKeyPressed(F3KEY_SMILE)?18:10;
		break;
	case BREATHEIN:
		CX=	(m_ChargePower< 40.0f?27:
			(m_ChargePower<120.0f?28:
			29));
		break;
	case BREATHEOUT:	CX=26; break;
	case TIRED:
		CX = ((m_PoseCounter+1)%40<20)?21:22;
		break;
	case FROZEN:	CX=23; break;
	case DEAD:		CX=13; break;
	case SMILE:		CX=18; break;
	}
	RECT rc = {CX*32+1, CY*32, CX*32+31, CY*32+30,};
	CDIB32* graphic = ResourceManager.Get(RID_MAIN);
	CDIB32* graphic2 = ResourceManager.Get(RID_MAINICY);
	lp->RotateBlt(m_nPower==0?graphic:graphic2,&rc,m_nVX,m_nVY,m_Angle,65536,4);
	if (m_Power<-1.0f/4096.0f) {
		rc.left=(m_PoseCounter2<20?0:64)+(floor(m_X/32)<m_pParent->GetWidth()-1?0:128);
		rc.top=96;
		rc.right=rc.left+64;
		rc.bottom=rc.top+32;
		lp->BltNatural(graphic,m_nVX-16,m_nVY,&rc);
	}
}

void Cf3MapObjectfff::OnMove()
{
	if (!IsValid()) return;
	if (!m_pParent->IsPlayable()) return;
	float Wind = m_pParent->GetWind(floor(m_X/32),floor(m_Y/32));
	if (m_pParent->ItemCompleted()) Smile();
	float ADX=m_X-m_OldX, ADY=m_Y-m_OldY, ADDX=m_DX-m_OldDX, ADDY=m_DY-m_OldDY;
	if (theSetting->m_Hyper) m_nPower=16;
	m_DAngle+=(ADX*ADDY-ADY*ADDX)*ROTATEACCEL;
	m_DAngle-=m_DAngle*ROTATEFRICTION;
	m_Angle+=m_DAngle;
	m_OldX = m_X; m_OldY = m_Y;
	m_OldDX = m_DX; m_OldDY = m_DY;
	// 動かしま〜す
	if (m_State==NORMAL) {
		// 空中
		float ddx=0, ddy=0;
		if (m_PowerX<=0 && m_pInput->GetKeyPressed(F3KEY_LEFT))		ddx-=1;
		if (m_PowerX>=0 && m_pInput->GetKeyPressed(F3KEY_RIGHT))	ddx+=1;
		if (m_PowerY<=0 && m_pInput->GetKeyPressed(F3KEY_UP))		ddy-=1;
		if (m_PowerY>=0 && m_pInput->GetKeyPressed(F3KEY_DOWN))		ddy+=1;
		if (ddx*ddx+ddy*ddy>FLYACCEL*FLYACCEL) {
			float r = FLYACCEL/sqrt(ddx*ddx+ddy*ddy);
			ddx *= r; ddy *= r;
		}
		for (int i=m_pInput->GetKeyPressed(F3KEY_JUMP)?0:1;i<=1;i++) {
			m_DX -= (m_DX-Wind)*FLYFRICTION;
			m_DX += ddx;
			m_DY -= m_DY*FLYFRICTION;
			m_DY += ddy;
		}
		if (m_pInput->GetKeyPushed(F3KEY_ATTACK)) BreatheIn();
	}ef(m_State==BREATHEIN) {
		// 冷気充填中
		m_ChargePower+=2.0f;
		float ddx=0, ddy=0;
		if (m_PowerX<=0 && m_pInput->GetKeyPressed(F3KEY_LEFT))		ddx-=1;
		if (m_PowerX>=0 && m_pInput->GetKeyPressed(F3KEY_RIGHT))	ddx+=1;
		if (m_PowerY<=0 && m_pInput->GetKeyPressed(F3KEY_UP))		ddy-=1;
		if (m_PowerY>=0 && m_pInput->GetKeyPressed(F3KEY_DOWN))		ddy+=1;
		if (ddx*ddx+ddy*ddy>FLYACCEL*FLYACCEL) {
			float r = FLYACCEL/sqrt(ddx*ddx+ddy*ddy);
			ddx *= r; ddy *= r;
		}
		m_DX -= (m_DX-Wind)*FLYFRICTION;
		m_DX += ddx;
		m_DY -= m_DY*FLYFRICTION;
		m_DY += ddy;
		if (!m_pInput->GetKeyPressed(F3KEY_ATTACK)) BreatheOut();
	}ef(m_State==BREATHEOUT) {
		// 冷気放出！！
		m_ChargePower-=1.0f;
		m_DX -= (m_DX-Wind)*FLYFRICTION;
		m_DY -= m_DY*FLYFRICTION;
		if (m_ChargePower<=0.0f) {
			if (m_nPower) {
				m_State=NORMAL;
			}else {
				Tire();
			}
		}
	}ef(m_State==TIRED) {
		// ちかれたー！
		m_PoseCounter--;
		m_DX -= (m_DX-Wind)*FLYFRICTION;
		m_DY -= m_DY*FLYFRICTION;
		if (m_PoseCounter==0) m_State=NORMAL;
	}ef(m_State==FROZEN) {
		// 凍っちゃった…
		m_PoseCounter--;
		m_DX -= (m_DX-Wind)*FLYFRICTION/5;
		m_DY -= m_DY*FLYFRICTION/5;
		if (m_PoseCounter==0) m_State=NORMAL;
	}
	// 速度飽和(めり込み防止)
	if (m_DX*m_DX+m_DY*m_DY>13*13) {
		float r = 13.0f/sqrt(m_DX*m_DX+m_DY*m_DY);
		m_DX *= r; m_DY *= r;
	}
	// 実際の移動+当たり判定
	// １回の移動ごとに当たり判定
	// という手順ですり抜けバグは解消されるはず
	m_X += m_DX;
	HitCheck();
	m_Y += m_DY;
	HitCheck();
}

void Cf3MapObjectfff::HitCheck()
{
	float mw=m_pParent->GetWidth()*32, mh=m_pParent->GetHeight()*32;
	int CX=floor(m_X/32),
		CL=floor((m_X-14)/32),
		CR=floor((m_X+14)/32),
		CY=floor(m_Y/32),
		CT=floor((m_Y-14)/32),
		CB=floor((m_Y+14)/32);
	if (m_DX>=0) {	// 右へ
		if (CR!=CX) {
			if (m_pParent->GetHit(CR,CY,HIT_LEFT)) {
				m_X = 18+32*CX;
				m_DX = 0;
			}
		}
	}
	if(m_DX<=0) {	// 左へ
		if (CL!=CX) {
			if (m_pParent->GetHit(CL,CY,HIT_RIGHT)) {
				m_X = 14+32*CX;
				m_DX = 0;
			}
		}
	}
	if (m_DY>=0) {	// 落ちるとき
		if (CB!=CY) {
			if (m_pParent->GetHit(CX,CB,HIT_TOP)) {
				m_Y = 18+32*CY;
				m_DY = 0;
			}
		}
	}ef(m_DY<0){	// 飛ぶとき
		if (CT!=CY) {
			if (m_pParent->GetHit(CX,CT,HIT_BOTTOM)) {
				m_Y = 14+32*CY;
				m_DY = 0;
			}
		}
	}
	if (m_pParent->GetHit(CX,CY,HIT_DEATH)) Die();
	if (m_Y+14 < 0) Die();
	if (m_Y-14 > mh) Die();
}

void Cf3MapObjectfff::Die()
{
	m_State=DEAD;
	m_DX = m_DY = 0;
}

void Cf3MapObjectfff::Synergy()
{
	if (m_State==DEAD||m_State==SMILE) return;
	m_Power = m_PowerX = m_PowerY = 0.0f;
	Cf3MapObjectBase**it;
	// ギヤバネ
	for(it=m_pParent->GetMapObjects(m_nCX-2, m_nCY-2, m_nCX+2, m_nCY+2, MOT_GEASPRIN); (*it)!=NULL; it++){
		if ((*it)->IsValid()) {
			float objX, objY;
			(*it)->GetPos(objX,objY);
			if (!((Cf3MapObjectGeasprin*)(*it))->IsFrozen()) {
				if (IsIn(objX-16,m_X,objX+15)) {
					if (IsIn(objY-30,m_Y,objY+16)) {
						theApp->GetBGM()->MusicEffect(MEN_GEASPRIN);
						m_DY=-10;
					}
				}ef(IsIn(objX+16,m_X,objX+29)) {
					if (IsIn(objY-16,m_Y,objY+15)) {
						theApp->GetBGM()->MusicEffect(MEN_GEASPRIN);
						m_DX=10;
					}
				}ef(IsIn(objX-29,m_X,objX-16)) {
					if (IsIn(objY-16,m_Y,objY+15)) {
						theApp->GetBGM()->MusicEffect(MEN_GEASPRIN);
						m_DX=-10;
					}
				}
			}else{
				if (IsIn(objX-16,m_X,objX+15)) {
					if (IsIn(objY-30,m_Y,objY)&&m_DY>=0) {
						m_Y = objY-30;
						m_DY=0;
					}
				}ef(IsIn(objX+16,m_X,objX+29)) {
					if (IsIn(objY-16,m_Y,objY+15)) {
						m_X = objX+30;
						m_DX=0;
					}
				}ef(IsIn(objX-29,m_X,objX-16)) {
					if (IsIn(objY-16,m_Y,objY+15)) {
						m_X = objX-30;
						m_DX=-0;
					}
				}
			}
		}
	}
	// とげとげ
	for(it=m_pParent->GetMapObjects(m_nCX-2, m_nCY-2, m_nCX+2, m_nCY+2, MOT_NEEDLE); (*it)!=NULL; it++){
		if ((*it)->IsValid()) {
			float objX, objY;
			(*it)->GetPos(objX,objY);
			if ((objX-m_X)*(objX-m_X)+(objY-m_Y)*(objY-m_Y)<256) {
				Die();
				return;
			}
		}
	}
	// ウナギカズラ
	for(it=m_pParent->GetMapObjects(m_nCX-2, m_nCY-2, m_nCX+2, m_nCY+2, MOT_EELPITCHER); (*it)!=NULL; it++){
		if ((*it)->IsValid()&&((Cf3MapObjectEelPitcher*)(*it))->IsLeaf()) {
			float objX, objY;
			(*it)->GetPos(objX,objY);
			if (IsIn(objX-16,m_X,objX+16)) {
				if (IsIn(objY-14,m_Y,objY)) {
					if (m_DY>=0) {
						m_Y = objY-14;
						m_DY=0;
					}
				}
			}
		}
	}
	if (m_State!=FROZEN) {
		// 氷
		for(it=m_pParent->GetMapObjects(m_nCX-2, m_nCY-2, m_nCX+2, m_nCY+2, MOT_ICE); (*it)!=NULL; it++){
			if ((*it)->IsValid()&&((Cf3MapObjectIce*)(*it))->GetSize()>10) {
				float objX, objY;
				(*it)->GetPos(objX,objY);
				if ((objX-m_X)*(objX-m_X)+(objY-m_Y)*(objY-m_Y)<256) {
					// あたった！
					Freeze(((Cf3MapObjectIce*)(*it))->GetSize());
				}
			}
		}
		// 氷ゾーン
		for(set<Cf3MapObjectIceSource*>::iterator is = Cf3MapObjectIceSource::IteratorBegin();
		is!=Cf3MapObjectIceSource::IteratorEnd();is++){
			float objX, objY;
			(*is)->GetPos(objX,objY);
			float dX = objX-m_X, dY = objY-m_Y,
				p=1.0f/(dX*dX+dY*dY), p3 = p*sqrt(p);
			m_Power += p;
			m_PowerX+= dX*p3;
			m_PowerY+= dY*p3;
		}
		// 炎ゾーン
		for(set<Cf3MapObjectFire*>::iterator fr = Cf3MapObjectFire::IteratorBegin();
		fr!=Cf3MapObjectFire::IteratorEnd();fr++){
			if ((*fr)->IsActive()) {
				float objX, objY;
				(*fr)->GetPos(objX,objY);
				float dX = objX-m_X, dY = objY-m_Y,
					p=1.0f/(dX*dX+dY*dY), p3 = p*sqrt(p);
				m_Power -= p;
				m_PowerX-= dX*p3;
				m_PowerY-= dY*p3;
			}
		}
		if (m_Power>1.0f/256.0f) {
			Freeze();
		}ef(m_Power>1.0f/4096.0f) {
			m_nPower=4;
			m_PowerX = m_PowerY = 0.0f;
		}ef(m_Power<-1.0f/256.0f) {
			Die();
		}ef(m_Power<-1.0f/4096.0f) {
		}else{
			m_PowerX = m_PowerY = 0.0f;
		}
	}
	// バナナ(BGMの調整用)
	if (m_pParent->GetMainChara()==this) {
		float bd, bananaDistance = 1e10;
		int nBanana=0, nPosition=0;
		int cx, cy;
		for(set<Cf3MapObjectBanana*>::iterator bn = Cf3MapObjectBanana::IteratorBegin();bn!=Cf3MapObjectBanana::IteratorEnd();bn++){
			if ((*bn)->IsValid()) {
				(*bn)->GetCPos(cx, cy);
				bd = (cx*32+16-m_X)*(cx*32+16-m_X)+(cy*32+16-m_Y)*(cy*32+16-m_Y);
				if (bd<bananaDistance) bananaDistance=bd;
				nBanana++;
				nPosition += cx-m_nCX;
			}
		}
		theApp->GetBGM()->MusicEffect(MEN_BANANADISTANCE, bananaDistance);
		theApp->GetBGM()->MusicEffect(MEN_BANANAPOSITION, nBanana?(float)nPosition/nBanana:0.0f);
	}
}

void Cf3MapObjectfff::Freeze(int level)
{
	m_State = FROZEN;
	m_PoseCounter = level*8;
}

void Cf3MapObjectfff::BreatheIn()
{
	if (m_nPower) {
		m_State = BREATHEIN;
		m_ChargePower = 0.0f;
	}
}

void Cf3MapObjectfff::BreatheOut()
{
	int p=floor(m_ChargePower/40.0f)+1;
	Saturate(1,p,m_nPower);
	int start=-m_Angle+(p-1)*16,angle;
	for (int i=0; i<p; i++) {
		angle = start-32*i;
		new Cf3MapObjectIce(
			m_X+CSinTable::Sin(angle)*16/65536,
			m_Y-CSinTable::Cos(angle)*16/65536,
			m_DX+CSinTable::Sin(angle)*6/65536,
			m_DY-CSinTable::Cos(angle)*6/65536);
	}
	m_nPower -= p;
	m_State = BREATHEOUT;
	m_ChargePower=10.0f;
}

void Cf3MapObjectfff::Tire()
{
	m_State = TIRED;
	m_PoseCounter = 100;
}
