// f3SceneFactory.cpp: Cf3SceneFactory クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3SceneFactory.h"
#include "SceneBase.h"
#include "FirstScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ReplayScene.h"
#include "ExplainScene.h"
#include "ConfigScene.h"
#include "SettingScene.h"
#include "ResultScene.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

/*Cf3SceneFactory::Cf3SceneFactory()
{

}*/

Cf3SceneFactory::~Cf3SceneFactory()
{

}

CScene* Cf3SceneFactory::CreateScene(int nScene) {
	CSceneBase* lp;
	switch ((SCENE)nScene){
	case NULL_SCENE:	lp = NULL; break;
	case FIRST_SCENE:	lp = new CFirstScene; break;
	case TITLE_SCENE:	lp = new CTitleScene; break;
	case GAME_SCENE:	lp = new CGameScene; break;
	case REPLAY_SCENE:	lp = new CReplayScene; break;
	case EXPLAIN_SCENE:	lp = new CExplainScene; break;
	case CONFIG_SCENE:	lp = new CConfigScene; break;
	case SETTING_SCENE:	lp = new CSettingScene; break;
	case RESULT_SCENE:	lp = new CResultScene; break;
	default:			lp = NULL; // error..
	}
//	lp->SetOutClass(&outer);
	return lp;
}
