// f3MapObjectfff.h: Cf3MapObjectfff �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAPOBJECTFFF_H__6882F256_7107_49DF_9AD8_BCAAB72BC836__INCLUDED_)
#define AFX_F3MAPOBJECTFFF_H__6882F256_7107_49DF_9AD8_BCAAB72BC836__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f3MapObjectMain.h"

class Cf3MapObjectfff : public Cf3MapObjectMain  
{
private:
	void Tire();
	void BreatheOut();
	void BreatheIn();
	void Freeze(int level=15);
	void Die();
	void HitCheck();
	void Smile() { m_State=SMILE; }

//	CDIB32* m_Graphic;
//	CDIB32* m_Graphic2;

	float m_OldX, m_OldY;
	float m_DX, m_DY, m_OldDX, m_OldDY;	// �ʒu�Ȃǂ̏��
	float m_Angle, m_DAngle;			// ��]�Ƃ�
	float m_ChargePower;				// �W�����v�`���[�W�W��(1.0f=100%���猸���Ă䂭)
	float m_Power,m_PowerX,m_PowerY;
	int m_nPower;
	enum f3fffState {
		NORMAL,
		BREATHEIN,
		BREATHEOUT,
		TIRED,
		FROZEN,
		DEAD,
		SMILE,
	} m_State;

	int m_PoseCounter, m_PoseCounter2;

	// �\���ʒu����
	int	m_VOffsetX, m_VOffsetY;
	int	m_VOffsetToX, m_VOffsetToY;
public:
	bool IsFrozen() { return m_State==FROZEN; }
	void Synergy();
	bool IsDied() { return m_State==DEAD; }
	void OnMove();
	void OnDraw(CDIB32 *lp);
	Cf3MapObjectfff(int nCX, int nCY);
	~Cf3MapObjectfff() {}

};

#endif // !defined(AFX_F3MAPOBJECTFFF_H__6882F256_7107_49DF_9AD8_BCAAB72BC836__INCLUDED_)
