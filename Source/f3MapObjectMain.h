// f3MapObjectMain.h: Cf3MapObjectMain クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAPOBJECTMAIN_H__A015BF21_DCB7_4F7B_A6CB_21FBE26C747E__INCLUDED_)
#define AFX_F3MAPOBJECTMAIN_H__A015BF21_DCB7_4F7B_A6CB_21FBE26C747E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f3MapObjectBase.h"
#include "App.h"

class CTextDIB32;

class Cf3GameInput
{
public:
	virtual bool GetKeyPushed	(int key)=0;
	virtual bool GetKeyPressed	(int key)=0;
};

extern class Cf3GameInputKey : public Cf3GameInput
{
public:
	bool GetKeyPushed	(int key) { return f3Input.GetKeyPushed(key); }
	bool GetKeyPressed	(int key) { return f3Input.GetKeyPressed(key); }
} KeyInput;

extern class Cf3GameInputReplay : public Cf3GameInput
{
public:
	BYTE pushed, pressed;
	bool GetKeyPushed	(int key) { return 0!=(pushed &(1<<(key-1))); }
	bool GetKeyPressed	(int key) { return 0!=(pressed&(1<<(key-1))); }
} ReplayInput;

class Cf3MapObjectMain : public Cf3MapObjectBase  
{
protected:
	static Cf3GameInput* m_pInput;
public:
	static SetInput(Cf3GameInput* pInput) { m_pInput = pInput; }
	static Cf3MapObjectMain* Create(int x, int y);
	virtual bool IsFrozen() { return false; }
	virtual void Die() {}
	virtual bool IsDied();
	virtual void GetViewPos(int &vx, int &vy);
	Cf3MapObjectMain(f3MapObjectType eType) : Cf3MapObjectBase(eType) {}
	virtual ~Cf3MapObjectMain() {}

};

#endif // !defined(AFX_F3MAPOBJECTMAIN_H__A015BF21_DCB7_4F7B_A6CB_21FBE26C747E__INCLUDED_)
