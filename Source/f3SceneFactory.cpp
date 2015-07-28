// f3SceneFactory.cpp: Cf3SceneFactory �N���X�̃C���v�������e�[�V����
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

//////////////////////////////////////////////////////////////////////
// �\�z/����
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
	default:			lp = NULL; // error..
	}
//	lp->SetOutClass(&outer);
	return lp;
}
