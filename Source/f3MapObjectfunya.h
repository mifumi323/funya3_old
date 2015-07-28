// f3MapObjectfunya.h: Cf3MapObjectfunya �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAPOBJECTFUNYA_H__9C96CCDF_38ED_4691_9336_B85CC4B26AE9__INCLUDED_)
#define AFX_F3MAPOBJECTFUNYA_H__9C96CCDF_38ED_4691_9336_B85CC4B26AE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f3MapObjectMain.h"

struct f3JumpFunction {
		float DY;
		float Power;
};

class Cf3MapObjectfunya : public Cf3MapObjectMain  
{
	friend  class Cf3MapObjectmrframe;
	friend  class CExplainScene;
protected:
	void Freeze(int level=15);
	void Tire();
	void BreatheOut();
	void BreatheIn();
	void Sit();
	void Sleep();
	void Blink();
	void HighJump();
	void Smile();
	void HitCheck();
	void Stop();
	void Run(f3MapObjectDirection direction);
	void Fall();
	void Land();
	void StartJump();
	void Jump();
	CDIB32* m_Graphic;
	CDIB32* m_Graphic2;

	float m_DX, m_DY;							// �ʒu�Ȃǂ̏��
	float m_BananaDistance;
	float m_ChargePower;						// �W�����v�`���[�W�W��(1.0f=100%���猸���Ă䂭)
	float m_ChargeDec;							// �W�����v�`���[�W�W�������l(�ݒ�ŕω�����\��)
	static const f3JumpFunction m_JumpFunc[4];	// �`���[�W�|�W�����v�͑Ή�
	float m_Power,m_PowerX,m_PowerY;
	int m_nPower;
	enum f3MainCharaState {
		STANDING,
		RUNNING,
		WALKING,
		CHARGING,
		JUMPING,
		BREATHEIN,
		BREATHEOUT,
		TIRED,
		DAMAGED,
		FROZEN,
		DEAD,
		SMILING,
		SLEEPING,
		BLINKING,
	} m_State;
	f3MapObjectDirection m_Direction;
	bool m_HitLeft, m_HitRight, m_HitTop, m_HitBottom, m_OnEnemy;
	bool m_bOriginal;
	bool m_bFirst;

	int m_PoseCounter, m_PoseCounter2, m_Sleepy;

	// �\���ʒu����
	int	m_VOffsetX,m_VOffsetY;
	int	m_VOffsetToX,m_VOffsetToY;
public:
	float GetGravity() { int g=theSetting->m_Gravity; return g==1?0.2f:(g==2?0.05f:0.1f); }
	bool IsFrozen() { return m_State==FROZEN; }
	void Die();
	bool IsDied();
	void GetViewPos(int &vx, int &vy);
	void Synergy();
	void OnPreDraw();
	void OnMove();
	virtual void OnDraw(CDIB32 *lp);
	Cf3MapObjectfunya(int nCX, int nCY);
	virtual ~Cf3MapObjectfunya();

};

#endif // !defined(AFX_F3MAPOBJECTFUNYA_H__9C96CCDF_38ED_4691_9336_B85CC4B26AE9__INCLUDED_)
