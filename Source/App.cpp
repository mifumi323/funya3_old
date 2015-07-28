// CApp
// ここから全てが始まる

#include "stdafx.h"
#include "App.h"
#include "yaneSDK/yaneAppInitializer.h"
#include "yaneSDK/yaneSingleApp.h"
#include "yaneSDK/yaneFPSTimer.h"
#include "yaneSDK/yaneFPSLayer.h"
#include "yaneSDK/yaneLineParser.h"
#include "yaneSDK/yaneTextDIB32.h"
#include "yaneSDK/yaneFile.h"
#include "f3SceneFactory.h"
#include "GameScene.h"
#include "ReplayScene.h"
#include "BGMDefault.h"
#include "BGMUser.h"
#include "f3MapObjectfunya.h"
#include "f3Map.h"
#include "ResourceManager.h"
#include "AVIRecorder.h"
#include <shlobj.h>

CRand CApp::m_Rand;
CApp* theApp;


CApp::CApp(){
	m_BGM = NULL;
	m_BGMMode = BGMM_NONE;
	m_AVI = NULL;
	m_Rand.Randomize();
	theApp = this;
}

CApp::~CApp(){
	DELETE_SAFE(m_BGM);
	DELETE_SAFE(m_AVI);
}

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(void){			//	これがワーカースレッド
		CApp().Start();						//	CApp app; app.Start();の意味ね
	}
	virtual LRESULT OnPreCreate(CWindowOption& opt){
		opt.caption		= "ふにゃさん";
		opt.classname	= "funya3";
		opt.size_x		= 320;
		opt.size_y		= 240;
		opt.style		= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		return 0;
	}
};

bool DirCheck() {
	CFile file;
	while (file.Read("resource/main.gif")) {
		if (MessageBox(NULL,
			"必要なファイルが見つかりません。\nふにゃさんをインストールしたフォルダを指定してください。",
			"エラー", MB_OKCANCEL)!=IDOK) return false;
		BROWSEINFO bInfo;
		LPITEMIDLIST pIDList;
		TCHAR szDisplayName[MAX_PATH];
    
		bInfo.hwndOwner = NULL;
		bInfo.pidlRoot = NULL;
		bInfo.pszDisplayName = szDisplayName;
		bInfo.lpszTitle = "フォルダの選択";
		bInfo.ulFlags = BIF_RETURNONLYFSDIRS;
		bInfo.lpfn = NULL;
		bInfo.lParam = (LPARAM)0;
    
		if((pIDList=SHBrowseForFolder(&bInfo))==NULL) return false;
		if(!SHGetPathFromIDList(pIDList, szDisplayName)) return false;
		CFile::SetCurrentDir(szDisplayName);

		CoTaskMemFree( pIDList );
	}
	return true;
}

//	言わずと知れたWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
	CAppInitializer::Init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	必ず書いてね
	if (!DirCheck()) return 0;
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CAppMainWindow().Run();					//	上で定義したメインのウィンドゥを作成
	}
	return 0;
}

void CApp::MainThread(){
	//  シーンコントローラーにSceneFactoryを渡してやる
	CSceneControl sc(smart_ptr<CSceneFactory>(new Cf3SceneFactory(this),true));
	int scene = FIRST_SCENE;
	// テストじゃないかな？
	{
		CLineParser l;
		l.SetLine(CAppInitializer::GetCmdLine());
		if (l.IsMatch("TEST")) {
			scene = GAME_SCENE;
			CGameScene::m_bTest = true;
			l.GetStr(m_StageFileName);
			m_StageFileName += ".f3s";
			l.GetNum(CGameScene::m_nStage);
		}ef(l.IsMatch("REPLAY")) {
			scene = REPLAY_SCENE;
			l.GetStr(m_StageFileName);
			m_StageFileName += ".f3r";
		}ef(l.IsMatch("RECORD")) {
			CReplayScene::record = true;
			m_Setting.m_FullScreen = false;
			scene = REPLAY_SCENE;
			l.GetStr(m_StageFileName);
			m_StageFileName += ".f3r";
		}
	}
	sc.JumpScene(scene);

	// 準備完了まで著作権表示
	GetDraw()->SetDisplay(m_Setting.m_FullScreen!=0,320,240);
	{
		CTextDIB32 tx;
		int w,h;
		tx.GetFont()->SetText("TokioSoft");
		tx.GetFont()->SetSize(32);
		tx.GetFont()->SetColor(0xffffff);
		tx.GetFont()->SetBackColor(0x303030);
		tx.UpdateText();
		tx.GetSize(w,h);
		GetDraw()->Blt(&tx,160-w/2,120-h/2);
		GetDraw()->OnDraw(NULL,false);
	}
	// で、準備
	CFPSTimer t;
	CFPSLayer l(&t);
	t.SetFPS((m_Setting.m_TimeMaster&&m_Setting.m_FPS)?m_Setting.m_FPS:40);
	m_BGM = new CBGMBase;
	m_BGMMode = BGMM_NONE;
	ChangeBGM((BGMMode)m_Setting.m_BGMMode);
	f3Input.Init(m_Setting.m_Key);
	Cf3Map::SetEffect(m_Setting.m_DrawMethod);
	Cf3MapObjectMain::SetInput(&KeyInput);
	ResourceManager.Init();

	while (IsThreadValid()) {
		f3Input.Input();
		if (GetForegroundWindow()==GetMyApp()->GetHWnd()) {
			if (f3Input.GetKeyPushed(F3KEY_EXIT)) break;
			// BGMの操作
			if (f3Input.GetKeyPushed(F3KEY_BGMNONE)) ChangeBGM(BGMM_NONE);
			if (f3Input.GetKeyPushed(F3KEY_BGMDEF)) ChangeBGM(BGMM_DEFAULT);
			if (f3Input.GetKeyPushed(F3KEY_BGMUSER)) ChangeBGM(BGMM_USER);
			m_BGM->Update();
			// CPlaneBaseじゃなくてCDIB32を使って呼び出す(これでCDIB32の全ての機能が使える)
			if (sc.OnDraw(GetDraw()->GetSecondary())) break;
			l.Enable(f3Input.GetKeyPressed(F3KEY_FPS));
			GetDraw()->OnDraw();
			if (f3Input.GetKeyPushed(F3KEY_CAPTURE)) ScreenCapture();
			if (m_AVI) m_AVI->Write(GetDraw()->GetSecondary());
		}
		t.WaitFrame();
	}
	ResourceManager.Quit();
}

void CApp::ChangeBGM(BGMMode no)
{
	if (m_BGM!=NULL&&m_BGMMode==no) return;
	CBGMBase* newBGM = NULL;
	BGMNumber num = m_BGM->GetBGM();
	switch (no) {
	case BGMM_NONE: {
		newBGM = new CBGMBase;
		break;
				   }
	case BGMM_DEFAULT: {
		newBGM = new CBGMDefault;
		break;
				   }
	case BGMM_USER: {
		newBGM = new CBGMUser;
		break;
				   }
	}
	if (newBGM==NULL) return;
	if (newBGM->Init()) {
		m_BGMMode=no;
		m_Setting.m_BGMMode = (int)m_BGMMode;
		DELETE_SAFE(m_BGM);
		m_BGM = newBGM;
		m_BGM->Play(num);
	}else{
		delete newBGM;
	}
}

void CApp::ScreenCapture()
{
	string filename;
	if (MakeFileName(filename, "bmp")) GetDraw()->GetSecondary()->Save(filename);
}

bool CApp::MakeFileName(string &filename, LPCSTR ext, int max, bool bForceToMake)
{
	char file[16];
	ULARGE_INTEGER oldest = { ULONG_MAX, ULONG_MAX };
	ULARGE_INTEGER t;
	for (int i=0; i<max; i++) {
		sprintf(file, "%03d.%s", i, ext);
		HANDLE h = ::CreateFile(CFile::MakeFullName(file).c_str(),
			GENERIC_READ,		// Read
			FILE_SHARE_READ,	// ReadOpenなら共有を許すのはマナー 
			NULL,				// security
			OPEN_EXISTING,		// 存在してなければエラー
			FILE_ATTRIBUTE_NORMAL,	//	ファイル属性
			NULL				// テンプレートファイル
			);
		if (h==INVALID_HANDLE_VALUE) {
			filename = file;
			return true;
		}else {
			if (bForceToMake) {
				if (::GetFileTime(h,NULL,NULL,(FILETIME*)&t)) {
					if (t.QuadPart<oldest.QuadPart) {
						oldest = t;
						filename = file;
					}
				}
			}
			::CloseHandle(h);
		}
	}
	if (!bForceToMake) return false;
	return !(oldest.HighPart==ULONG_MAX&&oldest.LowPart==ULONG_MAX);
}

void CApp::AVIRecordingStart()
{
	AVIRecordingStop();
	m_AVI = new CAVIRecorder;
}

void CApp::AVIRecordingStop()
{
	DELETE_SAFE(m_AVI);
}
