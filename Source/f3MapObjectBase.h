// f3MapObjectBase.h: Cf3MapObjectBase �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAPOBJECTBASE_H__5DC9A0BC_8508_43FD_AD28_B3DC25F7D8D8__INCLUDED_)
#define AFX_F3MAPOBJECTBASE_H__5DC9A0BC_8508_43FD_AD28_B3DC25F7D8D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Cf3Map;
class CDIB32;

enum f3MapObjectDirection {
	DIR_FRONT=0,
	DIR_LEFT=1,
	DIR_RIGHT=2,
};

enum f3MapObjectID {
	OID_FUNYA,
	OID_BANANA,
	OID_NEEDLE,
	OID_GEASPRIN,
	OID_EELPITCHER,
	OID_ICE,
	OID_ICESOURCE,
	OID_FIRE,
};

class Cf3MapObjectBase
{
protected:
	void SetViewPos(float offsetx=0, float offsety=0);
	float			m_X, m_Y;
	int				m_nVX, m_nVY;
	float			m_nScrollX, m_nScrollY;	// �X�N���[���ɂǂ�قǉe������邩%

	bool			m_bValid;
	int				m_nID;

	static Cf3Map*	m_pParent;
	static set<Cf3MapObjectBase*> m_CharaList;
public:
	static void KillAll();
	virtual void Synergy(){}				// �����I���ݍ�p(�K�����Ȋ������邱��)
	void GetCPos(int& x, int& y);
	float GetDistanceSquare(Cf3MapObjectBase& obj);
	float GetDistance(Cf3MapObjectBase& obj);
	static	void Garbage();
	static void SetParent(Cf3Map*lp);
	virtual void SynergyAll() {}
	virtual	void OnKillAll() {}
	virtual	void OnPreDrawAll() {}
	virtual	void OnDrawAll(CDIB32 *lp) {}
	virtual	void OnMoveAll() {}
	virtual	void OnPreDraw() {}
	virtual	void OnDraw(CDIB32 *lp) {}
	virtual	void OnMove() {}
	virtual	void Kill() { m_bValid = false; }
	bool	IsValid() { return m_bValid; }
	void SetPos(float x,float y) { m_X = x; m_Y = y; }
	void GetPos(float&x,float&y) { x = m_X; y = m_Y; }

	virtual	void	SetID(int nID) { m_nID = nID; }
	virtual	int		GetID() { return m_nID; }

	Cf3MapObjectBase();
	virtual ~Cf3MapObjectBase();

};

#endif // !defined(AFX_F3MAPOBJECTBASE_H__5DC9A0BC_8508_43FD_AD28_B3DC25F7D8D8__INCLUDED_)