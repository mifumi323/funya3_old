// ResourceManager.cpp: CResourceManager クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResourceManager.h"
#include "App.h"

class CResourceManager ResourceManager;

void CResourceManager::Init()
{
	// 全て同じresource.datから読み込むのでディスクキャッシュに入って速いはずだ！！！
	if (!m_Resource) m_Resource = new CDIB32[RID_END];
	switch (theSetting->m_Character) {
	case 1: m_Resource[RID_MAIN].Load("resource/glasses.gif"); break;
	case 45: m_Resource[RID_MAIN].Load("resource/anko.gif"); break;
	case 71: m_Resource[RID_MAIN].Load("resource/mrframe.gif"); break;
	default: m_Resource[RID_MAIN].Load("resource/main.gif"); break;
	}
	int sx, sy;
	m_Resource[RID_MAIN].GetSize(sx,sy);
	m_Resource[RID_MAINICY].CreateSurface(sx,sy);
	m_Resource[RID_MAINICY].BltFast(&m_Resource[RID_MAIN],0,0);
	m_Resource[RID_MAINICY].AddColorFast(DIB32RGB(0,32,32));
	m_Resource[RID_MAINICY].SubColorFast(DIB32RGB(64,0,0));
	m_Resource[RID_MAINICY].SetColorKey(0,0);
	m_Resource[RID_NEEDLE].Load("resource/Needle.gif");
	m_Resource[RID_GEASPRIN].Load("resource/Geasprin.gif");
	m_Resource[RID_EELPITCHER].Load("resource/EelPitcher.gif");
	m_Resource[RID_ICEFIRE].Load("resource/IceFire.yga");
	m_Resource[RID_MRFRAME].Load("resource/mrframe.gif");
	m_Resource[RID_EFFECT].Load("resource/Stars.gif");
	m_Resource[RID_NAVI].Load("resource/Navi.gif");
}
