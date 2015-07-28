// f3MapObjectMain.cpp: Cf3MapObjectMain クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectfff.h"
#include "f3MapObjectfunya.h"

Cf3GameInput* Cf3MapObjectMain::m_pInput=NULL;
Cf3GameInpuKey KeyInput;
Cf3GameInpuReplay ReplayInput;

void Cf3MapObjectMain::GetViewPos(int &vx, int &vy)
{
	vx = m_X;	vy = m_Y;
}

bool Cf3MapObjectMain::IsDied()
{
	return m_bValid;
}

Cf3MapObjectMain* Cf3MapObjectMain::Create(int x, int y)
{
	return (theSetting->m_Gravity!=3?
		(Cf3MapObjectMain*)new Cf3MapObjectfunya(x, y):
		(Cf3MapObjectMain*)new Cf3MapObjectfff(x, y));
}
