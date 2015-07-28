// f3MapObjectIceBase.cpp: Cf3MapObjectIceBase クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectIceBase.h"
#include "yaneSDK/yaneDIB32.h"
#include "ResourceManager.h"

/*CDIB32 Cf3MapObjectIceBase::m_Graphic;
bool Cf3MapObjectIceBase::m_bGraphicInitialize = false;*/

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3MapObjectIceBase::Cf3MapObjectIceBase()
{
/*	if (!m_bGraphicInitialize) {
		m_Graphic.Load("resource/IceFire.bmp");
		*m_Graphic.GetYGA() = true;
		CDIB32 dib;
		dib.Load("resource/IceFireMask.bmp");
		m_Graphic.BltToAlpha(&dib,0,255);
		m_bGraphicInitialize = true;
		m_Graphic.SaveYGA("resource/IceFire.yga",NULL,false);
	}
	m_Graphic.Open("resource/IceFire.yga");*/
	m_Graphic = ResourceManager.Get(RID_ICEFIRE);
}

Cf3MapObjectIceBase::~Cf3MapObjectIceBase()
{

}
