// ReplayScene.h: CReplayScene クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPLAYSCENE_H__6A688C07_02E2_458B_A8C9_54645E27AD8F__INCLUDED_)
#define AFX_REPLAYSCENE_H__6A688C07_02E2_458B_A8C9_54645E27AD8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneBase.h"

class Cf3Select;
class Cf3Replay;
class Cf3Map;

class CReplayScene : public CSceneBase  
{
protected:
	Cf3NavigationBar	*m_NavigationBar;
	TNavi				m_BananaGraphic;
	TTextNavi			m_BananaCount;
	TTextNavi			m_StageTitle;
	CTextDIB32			*m_Message;

	Cf3Replay			*m_pPlayer;
	Cf3Map				*m_pMap;
	int					m_Phase;
public:
	void OnDraw(CDIB32 *lp);
	void OnInit();
	CReplayScene();
	virtual ~CReplayScene();

	static bool record;
};

#endif // !defined(AFX_REPLAYSCENE_H__6A688C07_02E2_458B_A8C9_54645E27AD8F__INCLUDED_)
