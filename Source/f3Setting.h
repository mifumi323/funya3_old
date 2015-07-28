// f3Setting.h: Cf3Setting クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3SETTING_H__75299368_50AC_4F89_BD17_C6B8E2C0BB4C__INCLUDED_)
#define AFX_F3SETTING_H__75299368_50AC_4F89_BD17_C6B8E2C0BB4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f3Input.h"

extern class Cf3Setting  
{
protected:
	struct tagSetting {
		LPSTR	name;
		int*	pdata;
		bool	check;
	} *m_Data;
	int m_DataCount;

	map<string, int> m_Progress;

public:
	void SetProgress(string &file, int stage);
	int GetProgress(string &file);
	void InitSaveData();
	int GetChecksum();
	Cf3Setting();
	virtual ~Cf3Setting();

	int		m_BGMMode;
	int		m_FullScreen;
	int		m_Character;
	int		m_ViewTime;
	int		m_FPS;
	int		m_ESP;
	int		m_Gravity;
	int		m_RecordClear;
	int		m_RecordMiss;
	int		m_RecordNumber;
	int		m_Banana;
	int		m_PlayTime;
	int		m_SleepTime;
	int		m_Smiles;
	int		m_TimeMaster;
	int		m_Eyewitness;
	int		m_FeatherIron;
	int		m_GrapeColored;
	int		m_Esrever;
	int		m_DrawMethod;
	int		m_Outline;
	int		m_ColdMan;
	int		m_Hyper;
	int		m_AndBalloon;
	int		m_StartTime;
	int		m_Key[F3KEY_BUFSIZE];
}* theSetting;

#endif // !defined(AFX_F3SETTING_H__75299368_50AC_4F89_BD17_C6B8E2C0BB4C__INCLUDED_)
