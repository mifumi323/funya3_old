// SettingScene.h: CSettingScene クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGSCENE_H__F2831C4A_E735_4742_BE99_58B6F9DE2E33__INCLUDED_)
#define AFX_SETTINGSCENE_H__F2831C4A_E735_4742_BE99_58B6F9DE2E33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneBase.h"

class CSettingScene : public CSceneBase  
{
public:
	void OnDraw(CDIB32 *lp);
	void OnInit();
	CSettingScene();
	virtual ~CSettingScene();

};

#endif // !defined(AFX_SETTINGSCENE_H__F2831C4A_E735_4742_BE99_58B6F9DE2E33__INCLUDED_)
