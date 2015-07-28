// ExplainScene.h: CExplainScene クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPLAINSCENE_H__048DA514_83AD_4DBC_B82F_A86AE6042756__INCLUDED_)
#define AFX_EXPLAINSCENE_H__048DA514_83AD_4DBC_B82F_A86AE6042756__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneBase.h"

class Cf3Map;
class Cf3MapObjectfunya;

class CExplainScene : public CSceneBase  
{
private:
	Cf3Map				*m_Map;
//	Cf3NavigationBar	*m_NavigationBar;
	Cf3MapObjectfunya	*m_funya;
	TTextNavi			m_Text;

	int	m_nCounter;

	int m_OldGravity;
	int m_OldHyper;
public:
	void OnDraw(CDIB32 *lp);
	void OnInit();
	CExplainScene();
	virtual ~CExplainScene();

protected:
	int m_nSection;
	int m_nStep;
};

#endif // !defined(AFX_EXPLAINSCENE_H__048DA514_83AD_4DBC_B82F_A86AE6042756__INCLUDED_)
