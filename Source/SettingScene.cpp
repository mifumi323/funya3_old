// SettingScene.cpp: CSettingScene �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SettingScene.h"

#include "App.h"
#include "RandomReplay.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CSettingScene::CSettingScene()
{

}

CSettingScene::~CSettingScene()
{

}

void CSettingScene::OnInit()
{
	// ���v���C�֌W
	f3Replay.Enter();
	f3Navi.Clear();
}

void CSettingScene::OnDraw(CDIB32 *lp)
{
	lp->Clear(0);
	f3Replay.Replay(lp);
	f3Navi.OnDraw(lp);
}
