// f3Map.h: Cf3Map クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAP_H__9CF36D27_6AF2_45F8_87BE_D7E1B2F786B1__INCLUDED_)
#define AFX_F3MAP_H__9CF36D27_6AF2_45F8_87BE_D7E1B2F786B1__INCLUDED_

#include "BGMBase.h"	// ClassView によって追加されました。
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDIB32;
class Cf3StageFile;
class Cf3MapObjectBase;
class Cf3MapObjectMain;

const BYTE HIT_TOP		= 0x01;
const BYTE HIT_BOTTOM	= 0x02;
const BYTE HIT_LEFT		= 0x04;
const BYTE HIT_RIGHT	= 0x08;
const BYTE HIT_DEATH	= 0x10;

class Cf3Map  
{
	friend class Cf3MapObjectBanana;
	friend class CExplainScene;
private:
	CDIB32*	m_MapChip[3];
	BYTE*	m_MapData[3];
	BYTE	m_Width[3], m_Height[3];
	BYTE	m_Hit[240];
	BYTE	m_Stage;
	LONG	m_nGotBanana, m_nTotalBanana;

	string	m_Title;

	bool m_bPlayable;

	static BYTE		m_defHit[240];
	static float	m_Friction[8];

	BGMNumber		m_BGMNumber;

	int				m_ScrollX, m_ScrollY;
	float			m_ScrollRX, m_ScrollRY;

	float			*m_Wind;
	Cf3MapObjectBase**m_pObject;
	vector<Cf3MapObjectBase*> m_NearObject;

	Cf3MapObjectMain*	m_MainChara;

	static int m_nEffect;
	CDIB32* m_pDIBBuf;
public:
	Cf3MapObjectBase** GetMapObjects(int x1, int y1, int x2, int y2, int id);
	int GetIndex(int level, int x, int y) { return x+y*m_Width[level]; }
	int GetIndex(int x, int y) { return x+y*m_Width[1]; }
	void AddMapObject(int x, int y, Cf3MapObjectBase *p);
	void RemoveMapObject(int x, int y, Cf3MapObjectBase*p);
	CDIB32* ReadMapChip(Cf3StageFile*lp, int level);
	bool IsPlayable() { return m_bPlayable; }
	BYTE GetHeight(int level=1) { return m_Height[level]; }
	BYTE GetWidth(int level=1) { return m_Width[level]; }
	bool ItemCompleted() { return m_nGotBanana==m_nTotalBanana; }
	static void SetEffect(int effect) { m_nEffect=effect; }
	void GetMainCharaCPos(int &x, int &y);
	LRESULT SetMapData(int level, int x, int y, BYTE data);
	void CreateTemparatureMap(CDIB32* dib);
	float GetWind(int x,int y);
	Cf3MapObjectMain* GetMainChara() { return m_MainChara; }
	BGMNumber GetBGM() { return m_BGMNumber; }
	static long GetChunkType(long type, int stage);
	bool IsMainCharaDied();
	string GetTitle() { return m_Title; }
	long GetTotalBanana() { return m_nTotalBanana; }
	long GetGotBanana() { return m_nGotBanana; }
	void KillAllMapObject();
	void GarbageMapObject();
	void OnPreDraw();
	float GetFriction(int x, int y);
	void GetViewPos(int &x, int &y, float scrollx=1.0f, float scrolly=1.0f);
	void OnMove();
	BYTE GetMapData(int level, int x, int y);
	bool GetHit(int x, int y, BYTE hit);
	void OnDraw(CDIB32 *lp);
	Cf3Map(Cf3StageFile*lp,int stage,bool playable=true);
	virtual ~Cf3Map();

};

#endif // !defined(AFX_F3MAP_H__9CF36D27_6AF2_45F8_87BE_D7E1B2F786B1__INCLUDED_)
