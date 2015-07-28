// FirstScene.h: FirstScene クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIRSTSCENE_H__E487697A_A9AD_4332_8791_4FD2F4ECDC87__INCLUDED_)
#define AFX_FIRSTSCENE_H__E487697A_A9AD_4332_8791_4FD2F4ECDC87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneBase.h"

class Cf3Select;
class Cf3Replay;
class Cf3Map;

class CFirstScene : public CSceneBase  
{
protected:
	Cf3Select			*m_Select;
	Cf3NavigationBar	*m_NavigationBar;

	CTextDIB32			*m_TextTitle;
	CDIB32				*m_Icon;
	TTextNavi			m_TextNavi;
	TTextNavi			m_TextDemo;

	vector<string>		m_ReplayList;
	Cf3Replay			*m_pPlayer;
	Cf3Map				*m_pMap;
	int					m_Phase;
public:
	void Replay(CDIB32 *lp);
	void FindAllReplayFile();
	void OnDraw(CDIB32 *lp);
	void OnInit();
	CFirstScene();
	virtual ~CFirstScene();

};

#endif // !defined(AFX_FIRSTSCENE_H__E487697A_A9AD_4332_8791_4FD2F4ECDC87__INCLUDED_)
