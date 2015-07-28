// TitleScene.h: CTitleScene クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TITLESCENE_H__960461C3_A8DF_41D9_B46C_B82082A6286B__INCLUDED_)
#define AFX_TITLESCENE_H__960461C3_A8DF_41D9_B46C_B82082A6286B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneBase.h"

class Cf3Map;
class Cf3StageFile;
class Cf3Select;

class CTitleScene : public CSceneBase  
{
private:
	Cf3Map					*m_Map;
	Cf3StageFile			*m_StageFile;
	Cf3Select				*m_Select;
	Cf3NavigationBar		*m_NavigationBar;

	list<string>			m_StageList;
	list<string>::iterator	m_itStageFile;

	CTextDIB32				*m_TextStageFile;
	TTextNavi				m_TextNavi;

	int						m_nPenalty;
public:
	void SetMap(int map);
	void OnDraw(CDIB32 *lp);
	void OnInit();
	CTitleScene();
	virtual ~CTitleScene();

protected:
	void SetStage();
	void FindAllStageFile();
};

#endif // !defined(AFX_TITLESCENE_H__960461C3_A8DF_41D9_B46C_B82082A6286B__INCLUDED_)
