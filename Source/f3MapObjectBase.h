// f3MapObjectBase.h: Cf3MapObjectBase クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAPOBJECTBASE_H__5DC9A0BC_8508_43FD_AD28_B3DC25F7D8D8__INCLUDED_)
#define AFX_F3MAPOBJECTBASE_H__5DC9A0BC_8508_43FD_AD28_B3DC25F7D8D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Cf3Map;
class CDIB32;
class Cf3MapObjectBase;

enum f3MapObjectDirection {
	DIR_FRONT=0,
	DIR_LEFT=1,
	DIR_RIGHT=2,
};

enum f3MapObjectType {
//	MOT_UNKNOWN,

	MOT_FUNYA,
	MOT_BANANA,
	MOT_NEEDLE,
	MOT_GEASPRIN,
	MOT_EELPITCHER,
	MOT_ICE,
	MOT_ICESOURCE,
	MOT_FIRE,
	MOT_EFFECT,

//	MOT_COUNT
};

typedef set<Cf3MapObjectBase*> MapObjectList;

class Cf3MapObjectBase
{
//	friend class Cf3MapObjectmrframe;
//	friend class Cf3MapObjectEffect;
private:
	bool			m_bValid;
	f3MapObjectType	m_eType;
	int				m_nID;

	static int		m_nNextID;
	static MapObjectList m_CharaList;
protected:
	static void RemoveCharaFromList(Cf3MapObjectBase*lp) { m_CharaList.erase(lp); }
	void SetViewPos(float offsetx=0, float offsety=0);
//	void KillSimple() { m_bValid = false; }
	float			m_X, m_Y;
	int				m_nVX, m_nVY;
//	float			m_nScrollX, m_nScrollY;	// スクロールにどれほど影響されるか(100%固定なので省略)
	int				m_nCX, m_nCY;

	static Cf3Map*	m_pParent;
public:
	static int Count() { return m_CharaList.size();}
	static void UpdateCPosAll();
	virtual void UpdateCPos();
	static void KillAll();
	virtual void Synergy(){}				// 自発的相互作用(必ず自己完結すること)
	virtual void GetCPos(int& x, int& y);
	float GetDistanceSquare(Cf3MapObjectBase& obj);
//	float GetDistance(Cf3MapObjectBase& obj);
	static void Garbage();
	static void SetParent(Cf3Map*lp) { m_pParent = lp; }
	virtual	void OnPreDraw() {}
	virtual	void OnDraw(CDIB32 *lp) =0;
	virtual	void OnMove() {}
	void Kill();
	bool IsValid() { return m_bValid; }
	void SetPos(float x,float y) { m_X = x; m_Y = y; }
	void GetPos(float&x,float&y) { x = m_X; y = m_Y; }

	int GetID() { return m_nID; }
	int GetType() { return m_eType; }

	Cf3MapObjectBase(f3MapObjectType eType);
	virtual ~Cf3MapObjectBase();

	Cf3MapObjectBase* m_pNext;

};

#endif // !defined(AFX_F3MAPOBJECTBASE_H__5DC9A0BC_8508_43FD_AD28_B3DC25F7D8D8__INCLUDED_)
