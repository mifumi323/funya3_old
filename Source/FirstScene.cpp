// FirstScene.cpp: FirstScene クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FirstScene.h"

#include "GameScene.h"
#include "App.h"
#include "f3Select.h"
//#include "f3Replay.h"
#include "Version.h"
#include "ResourceManager.h"
#include "f3MapObjectMain.h"
#include "RandomReplay.h"
#include "yaneSDK/yaneDir.h"
#include "yaneSDK/yaneFile.h"

enum {
	FSM_GAMESTART,
	FSM_EXPLAIN,
	FSM_CONFIG,
	FSM_EXIT,

	FSM_RESULT,

	FSM_MAX
};

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CFirstScene::CFirstScene()
{
	m_Select = NULL;
	m_NavigationBar = NULL;
	m_TextTitle = NULL;
	m_TextNavi.dib = NULL;
	m_TextDemo.dib = NULL;
//	m_pPlayer = NULL;
}

CFirstScene::~CFirstScene()
{
	DELETE_SAFE(m_Select);
	DELETE_SAFE(m_NavigationBar);
	DELETE_SAFE(m_TextTitle);
	DELETE_SAFE(m_TextNavi.dib);
	DELETE_SAFE(m_TextDemo.dib);
//	DELETE_SAFE(m_pPlayer);
}

void CFirstScene::OnInit()
{
	// メインメニュー
	m_Select = new Cf3Select;
	m_NavigationBar = new Cf3NavigationBar;
	m_Select->SetPos(80,80);
	SetMenu();
	m_TextTitle = new CTextDIB32;
	m_TextTitle->GetFont()->SetSize(32);
	m_TextTitle->GetFont()->SetColor(0xffffff);
	m_TextTitle->GetFont()->SetBackColor(0x303030);
	m_TextTitle->GetFont()->SetText("ふにゃさん");
	m_TextTitle->UpdateText();
	// ナビゲーション
	m_TextNavi.Set(new CTextDIB32);
//	m_TextNavi.text->GetFont()->SetSize(16);
//	m_TextNavi.text->GetFont()->SetColor(0x000020);
//	m_TextNavi.text->GetFont()->SetBackColor(0x303030);
	{
		string navi="　　TokioSoft　ふにゃさん";
		char strbuf[256];
		navi += "　著作権：美文(ときお)";
		sprintf(strbuf,"　バージョン：%s",CVersion().GetVersion());
		navi += strbuf;
		sprintf(strbuf,"　%sを押してスタート",f3Input.GetKeyName(F3KEY_PAUSE));
		navi += strbuf;
		m_TextNavi.text->GetFont()->SetText(navi);
	}
	m_TextNavi.Update();
	m_NavigationBar->Add(&m_TextNavi);
	m_TextDemo.Set(new CTextDIB32);
//	m_TextDemo.text->GetFont()->SetSize(16);
//	m_TextDemo.text->GetFont()->SetColor(0x000020);
//	m_TextDemo.text->GetFont()->SetBackColor(0x303030);
	m_TextDemo.text->GetFont()->SetText("");
	m_TextDemo.Update();
	m_NavigationBar->Add(&m_TextDemo);
	// リプレイ関係
	f3Replay.Enter();
//	FindAllReplayFile();
	// その他
	m_Icon = ResourceManager.Get(RID_MAIN);
	theApp->GetBGM()->Play(BGMN_TITLE);
	CGameScene::m_bTest = false;
}

void CFirstScene::OnDraw(CDIB32 *lp)
{
	lp->Clear(0);
	if (f3Input.GetKeyPushed(F3KEY_PAUSE)) {
		if (m_Select->GetSelected()==FSM_GAMESTART) {
			// ゲーム開始
			f3Replay.Leave();
			f3Input.Input();
			JumpScene(TITLE_SCENE);
		}ef(m_Select->GetSelected()==FSM_EXPLAIN) {
			// 操作説明
			f3Replay.Leave();
			f3Input.Input();
			JumpScene(EXPLAIN_SCENE);
		}ef(m_Select->GetSelected()==FSM_CONFIG) {
			// 設定
			f3Input.Input();
			JumpScene(CONFIG_SCENE);
/*			theApp->GetDraw()->SetDisplay(false, 320, 240);
			Cf3ConfigWindow().Open();
			theApp->GetDraw()->SetDisplay(theSetting->m_FullScreen!=0, 320, 240);
			f3Input.Input();*/
		}ef(m_Select->GetSelected()==FSM_EXIT) {
			// 終了
			f3Replay.Leave();
			ExitScene();
		}ef(m_Select->GetSelected()==FSM_RESULT) {
			// プレイ記録
			f3Input.Input();
			JumpScene(RESULT_SCENE);
		}
	}else {
		int mnu=m_nMenu;
		if (f3Input.GetKeyPushed(F3KEY_LEFT)) mnu--;
		if (f3Input.GetKeyPushed(F3KEY_RIGHT)) mnu++;
		if (mnu!=m_nMenu) SetMenu(mnu);
	}
	f3Replay.Replay(lp, &m_TextDemo);
	m_Select->Enable(!f3Input.GetKeyPressed(F3KEY_SMILE));
	if (!f3Input.GetKeyPressed(F3KEY_SMILE)) {
		lp->Blt(m_TextTitle,80,32);
		RECT rc = { 256, 96, 288, 128 };
		lp->Blt(m_Icon,48,32,&rc);
	}
	m_NavigationBar->OnDraw(lp);
}

//DEL void CFirstScene::FindAllReplayFile()
//DEL {
//DEL 	CDir dir;
//DEL 	string file;
//DEL 	list<string> replayList;
//DEL 	dir.SetPath(CFile::GetCurrentDir());
//DEL 	dir.SetFindFile("*.f3r");
//DEL 	while (!dir.FindFile(file)) replayList.push_back(file);
//DEL 	m_ReplayList.resize(replayList.size());
//DEL 	int i=0;
//DEL 	for (list<string>::iterator it=replayList.begin(); it!=replayList.end(); it++)
//DEL 		m_ReplayList[i++] = *it;
//DEL }

//DEL void CFirstScene::Replay(CDIB32 *lp)
//DEL {
//DEL 	if (m_Phase) m_Phase++;
//DEL 	if (m_ReplayList.size()==0) return;
//DEL 	if (m_pPlayer==NULL) m_pPlayer = new Cf3Replay();
//DEL 	if (m_pPlayer->Finished()) {
//DEL 		if (m_Phase>=80) {
//DEL 			m_pPlayer->Load(m_ReplayList[CApp::random(m_ReplayList.size())]);
//DEL 			string navi="";
//DEL 			navi += "　　デモプレイ：";
//DEL 			navi += CFile::GetPureFileNameOf(m_pPlayer->GetFileName());
//DEL 			
//DEL 			m_TextDemo.text->GetFont()->SetText(navi);
//DEL 			m_TextDemo.Update();
//DEL 			m_Phase=-60;
//DEL 		}ef(!m_Phase) {
//DEL 			m_Phase++;
//DEL 		}
//DEL 	}
//DEL 	if (!m_Phase) m_pPlayer->Replay();
//DEL 	m_pPlayer->OnDraw(lp);
//DEL 	int fade=600-abs(10*m_Phase);
//DEL 	Saturate(0,fade,200);
//DEL 	lp->FadeEffect(fade);
//DEL }

void CFirstScene::SetMenu(int no)
{
	int sel = m_Select->GetSelectedPos();
	m_nMenu = no;
	if (m_nMenu<0) m_nMenu = (FSM_MAX+3)/4-1;
	ef (m_nMenu>(FSM_MAX+3)/4-1) m_nMenu = 0;
	m_Select->Clear();
	if (m_nMenu==0) {
		m_Select->Add("ゲーム開始", FSM_GAMESTART);
		m_Select->Add("操作説明", FSM_EXPLAIN);
//		m_Select->Add("設定", FSM_CONFIG);
		m_Select->Add("終了", FSM_EXIT);
	}ef(m_nMenu==1) {
		m_Select->Add("プレイ記録", FSM_RESULT);
	}
	m_Select->SelectPos(sel);
}
