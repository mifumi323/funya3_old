// GameScene.h: CGameScene クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMESCENE_H__681860B8_2D6B_4F94_BD6D_D6026813D206__INCLUDED_)
#define AFX_GAMESCENE_H__681860B8_2D6B_4F94_BD6D_D6026813D206__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneBase.h"
#include "f3NavigationBar.h"

class Cf3Map;
class Cf3StageFile;
class Cf3Select;
class Cf3Replay;

class CGameScene : public CSceneBase  
{
private:
	Cf3Map			*m_Map;
	Cf3StageFile	*m_StageFile;
	Cf3NavigationBar*m_NavigationBar;
	Cf3Select		*m_Select;
	CDIB32			*m_Temparature;

	int				m_nStageCount;
	bool			m_bSecret4;

	int				m_nRest;
	int				m_nPenalty;

	// ナビゲーション
	TNavi			m_BananaGraphic;
	TTextNavi		m_BananaCount;
	TTextNavi		m_StageTitle;
	TNavi			m_RestGraphic;
	TTextNavi		m_RestCount;

	CTextDIB32		*m_Message;
	int				m_MessageCount;

	Cf3Replay		*m_pRecorder;

	enum f3GameSceneState {
		GSS_PLAYING,
		GSS_PAUSED,
		GSS_CLEARED,
		GSS_ALLCLEARED,
		GSS_DEAD,
		GSS_GAMEOVER,
	} m_State;
public:
	void SaveReplay();
	virtual void OnDraw(CDIB32*lp);
	virtual void OnInit();
	CGameScene();
	virtual ~CGameScene();

	static bool	m_bTest;
	static int	m_nStage;

protected:
	void Unpause();
	void Pause();
	void UpdateRest();
	void RestartGame(int stage);
	void GameOver();
	void Ending();
	void Miss();
	void StageClear();
	void StartGame(int stage);
};

#endif // !defined(AFX_GAMESCENE_H__681860B8_2D6B_4F94_BD6D_D6026813D206__INCLUDED_)
