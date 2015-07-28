// SettingScene.cpp: CSettingScene クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SettingScene.h"

#include "App.h"
#include "RandomReplay.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CSettingScene::CSettingScene()
{

}

CSettingScene::~CSettingScene()
{

}

void CSettingScene::OnInit()
{
	// リプレイ関係
	f3Replay.Enter();
	f3Navi.Clear();
}

void CSettingScene::OnDraw(CDIB32 *lp)
{
	lp->Clear(0);
	f3Replay.Replay(lp);
	f3Navi.OnDraw(lp);
}
