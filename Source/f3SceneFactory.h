// f3SceneFactory.h: Cf3SceneFactory クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3SCENEFACTORY_H__9A734DBA_1C1A_49C8_B1CA_87DA54C6F17A__INCLUDED_)
#define AFX_F3SCENEFACTORY_H__9A734DBA_1C1A_49C8_B1CA_87DA54C6F17A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "YANESDK\yaneScene.h"
#include "App.h"

class Cf3SceneFactory : 
	public CSceneFactory, 
	public mediator<CApp>  
{
public:
	Cf3SceneFactory(CApp* pv) : mediator<CApp>(pv){}
	virtual ~Cf3SceneFactory();
protected:
	virtual CScene* CreateScene(int nScene);
};

#endif // !defined(AFX_F3SCENEFACTORY_H__9A734DBA_1C1A_49C8_B1CA_87DA54C6F17A__INCLUDED_)
