// SceneBase.h: CSceneBase クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEBASE_H__97855B08_8F0E_47BF_BA01_EC5ED955F708__INCLUDED_)
#define AFX_SCENEBASE_H__97855B08_8F0E_47BF_BA01_EC5ED955F708__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "YANESDK\yaneScene.h"
#include "YANESDK\yaneDIB32.h"
#include "f3NavigationBar.h"
//#include "App.h"

enum SCENE {
	NULL_SCENE,
	FIRST_SCENE,
	TITLE_SCENE,
	GAME_SCENE,
	REPLAY_SCENE,
	EXPLAIN_SCENE,
};

class CSceneBase : 
	public CScene/*, 
	public mediator<CApp>  */
{
public:
	CSceneBase() {}
	virtual ~CSceneBase() {}

	void OnDraw(CPlaneBase *lp) { OnDraw((CDIB32*)lp); }
	virtual void OnDraw(CDIB32 *lp) { CScene::OnDraw(lp); }

};

#endif // !defined(AFX_SCENEBASE_H__97855B08_8F0E_47BF_BA01_EC5ED955F708__INCLUDED_)
