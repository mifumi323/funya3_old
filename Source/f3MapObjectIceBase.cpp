// f3MapObjectIceBase.cpp: Cf3MapObjectIceBase クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MapObjectIceBase.h"
#include "yaneSDK/yaneDIB32.h"
#include "ResourceManager.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3MapObjectIceBase::Cf3MapObjectIceBase(f3MapObjectType eType)
	:Cf3MapObjectBase(eType)
{
	m_Graphic = ResourceManager.Get(RID_ICEFIRE);
}

Cf3MapObjectIceBase::~Cf3MapObjectIceBase()
{

}
