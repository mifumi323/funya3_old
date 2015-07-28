// CApp

#if !defined(__APP_H__)
#define __APP_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "yaneSDK/yaneAppFrame.h"
#include "yaneSDK/yaneDIBDraw.h"
#include "yaneSDK/yaneFPSTimer.h"
#include "yaneSDK/yaneRand.h"
#include "f3Setting.h"
#include "BGMBase.h"

enum {
	F3CODE_EXIT,
	F3CODE_TITLE,
	F3CODE_GAME,
};

class CAVIRecorder;

extern class CApp : public CAppFrame  {
public:
	void SetFullScreen(bool bFS);
	void AVIRecordingStop();
	void AVIRecordingStart();
	static bool MakeFileName(string &filename, LPCSTR ext, int max=1000, bool bForceToMake=false);
	void ScreenCapture();
	void ChangeBGM(BGMMode no);
	CApp();
	virtual ~CApp();

	CDIB32*		GetDIB()		{ return theSetting->m_Zoom?&m_Backup:m_Draw.GetSecondary(); }
	CBGMBase*	GetBGM()		{ return  m_BGM; }
	CFPSTimer*	GetFPS()		{ return  m_FPS; }
	static int	random(int n) { return m_Rand.Get(n); }
	static int	random(int min, int max) { return m_Rand.Get(max-min+1)+min; }
	static bool	binaryrandom(int num, int den=100) { return m_Rand.Get(den)<num; }
	void		SetStageFile(string &s)	{ m_StageFileName = s; }
	string		GetStageFile() { return m_StageFileName; }
	void		SetAutoDraw(bool b=true) { m_AutoDraw = b; }
protected:
	void InitResource();
	void MainThread();

	CDIBDraw		m_Draw;
	CDIB32			m_Backup;
	static CRand	m_Rand;
	CBGMBase*		m_BGM;
	BGMMode			m_BGMMode;
	CAVIRecorder*	m_AVI;
	Cf3Setting		m_Setting;
	CFPSTimer*		m_FPS;

	string			m_StageFileName;
	bool			m_AutoDraw;
}* theApp;

#endif